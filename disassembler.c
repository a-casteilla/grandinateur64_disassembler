/*******************************************************************************
 * File name : disassembler.c
 * Author : Aurélien Casteilla
 * Version 0.1 
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mnemonic.h"
#include "config.h"

/* made address_width a global variable because it's the same everywhere */
int address_width = DEFAULT_ADDRESS_WIDTH;
/* See above */
uint64_t address_width_mask = DEFAULT_ADDRESS_WIDTH_MASK;
/* to have the right number of hex digits */
int address_width_nibble = DEFAULT_ADDRESS_WIDTH_NIBBLE;

/* flag for the --mnemonic-only option */
bool mnemonic_only_flag = false;

/* starting address of the program to disassemble */
uint64_t start_address = 0;
/* ending address of the program to disassemble */
uint64_t end_address = 0;

/* flag used when --rom or --end-address is used */
bool force_end_flag = false;

char * disassemble_word (uint32_t instruction_word, uint64_t current_pc) {

    char * disassembled_line = (char*)malloc(1024) ; /* Returned string */
    bool is_found = false;             /* Become true when a correspondance is
                                          found to exit the loops */
    size_t i;                          /* is used inside and after the for
                                          loops */
    int rd;                            /* destination register */
    int rx;                            /* first source */
    int ry;                            /* second source */
    int shift;                         /* shift amount for the operand */
    uint32_t imm_value;                /* immediate operand for ALU operations */
    char str_format[16];               /* format string for sprintf */
    char str_to_append[1024];          /* used to avoid using sprinft like strcat */
    uint64_t offset;                   /* used to represent a relative address */

    /* firstly, test if the instruction is a branch because I don't want to see
     * EQ.BAL instead of BEQ */
    if (is_branch_instruction(instruction_word)) {

        /* Loop through the mnemonics until a correspondance is found with the
         * current instruction word */
        for (i = 0; i < sizeof(mnemo)/sizeof(mnemo[0]) && !is_found; i++)
            is_found = !((instruction_word & BRANCH_MASK) ^ mnemo[i].word);
        i--; /* necessary because the i is incremented at the end */

        /* Verify if a correspondance was found, if a correspondance was not
         * found, print ILG */
        if (!is_found) {
            strcpy(disassembled_line, "ILG");
            return disassembled_line;
        } else {
            offset = (BRANCH_OFF_MASK & instruction_word) >> 3;
            strcpy(disassembled_line, mnemo[i].mnemo);
            strcat(disassembled_line, " ");
            rd = extract_rd(instruction_word);
            /* Doesn't print "BEQ PC, $abcd" but prints "BEQ
             * $an_absolute_address" */
            if (rd == PC) {
                /* offset is sign extended for arithmetic purpose */
                if (offset & 0x8000)
                    offset |= 0xffffffffffff0000;
                sprintf(str_format, "$%%.%dlx", address_width_nibble);
                sprintf(str_to_append, str_format, (current_pc + offset + 4) &
                        address_width_mask);
                strcat(disassembled_line, str_to_append);
                return disassembled_line;
            } else {
                strcat(disassembled_line, reg_name[rd]);
                if (offset) {
                    if (rd) {
                        sprintf(str_to_append, ", $%.4lx", offset);
                        strcat(disassembled_line, str_to_append);
                    } else { /* Prints an address if the register is R0 */
                        /* offset is sign extended to have a full address */
                        if (offset & 0x8000)
                            offset |= 0xffffffffffff0000;
                        sprintf(str_format, ", $%%.%dlx", address_width_nibble);
                        sprintf(str_to_append, str_format, offset &
                                address_width_mask);
                        strcat(disassembled_line, str_to_append);
                    }
                }
                return disassembled_line;
            }
        }
    } else if (is_memory_instruction(instruction_word)) {

        /* Verify if there is a conditionnal prefix or not */
        if (extract_condition(instruction_word)) {
            for (i = 0; i < sizeof(prefix)/sizeof(prefix[0]) && !is_found; i++)
                is_found = !(extract_condition(instruction_word) ^ prefix[i].word);
            i--;
            if (!is_found) {
                strcpy(disassembled_line, "ILG");
                return disassembled_line;
            } else /* prints the corresponding prefix */
                strcpy(disassembled_line, prefix[i].prefix);
        }

        /* Searches the corresponding mnemonic */
        is_found = false;
        for (i = 0; i < sizeof(mnemo)/sizeof(mnemo[0]) && !is_found; i++)
            is_found = !((MEMORY_MASK & instruction_word) ^ mnemo[i].word);
        i--;
        if (!is_found) { /* Should not happen but it's there in case of a problem */
            strcpy(disassembled_line, "ILG");
            return disassembled_line;
        }
        /* prints the corresponding mnemonic */
        strcat(disassembled_line, mnemo[i].mnemo);
        /* checks for the suffix bit */
        if (is_suffix(instruction_word)) strcat(disassembled_line, "S ");
        else strcat(disassembled_line, " ");
        /* Appends the destination register */
        strcat(disassembled_line, reg_name[extract_rd(instruction_word)]);
        /* Get the addressing mode to format the output */
        rx = extract_rx(instruction_word);
        ry = extract_ry(instruction_word);
        switch (instruction_word & MEMORY_MODE_MASK) {
            case 0x0:
                /* direct mode, output should look like "FOO Rd, $3abc" */
                offset = (MEMORY_DIRECT_MASK & instruction_word) >> 5;
                /* offset is sign extended to have an address format*/
                if (offset & 0x4000)
                    offset |= 0xffffffffffff8000;
                sprintf(str_format, ", $%%.%dlx", address_width_nibble);
                sprintf(str_to_append, str_format, offset & address_width_mask);
                strcat(disassembled_line, str_to_append);
                return disassembled_line;
            case 0x00200000:
                /* indexed mode, output should look like "FOO Rd, Rx, $3ff" */
                offset = (MEMORY_I_OFF_MASK & instruction_word) >> 10;
                if (rx == PC) {
                    /* I want to see "FOO PC, $absolute". I kept PC in the
                     * format to avoid confusion with the direct addressing
                     * mode */
                    /* offset is sign extended for arithmetic purpose */
                    if (offset & 0x200)
                        offset |= 0xfffffffffffffc00;
                    sprintf(str_format, ", PC, $%%.%dlx", address_width_nibble);
                    sprintf(str_to_append, str_format, 
                            (current_pc + offset + 4) & address_width_mask);
                    strcat(disassembled_line, str_to_append);
                } else {
                    strcat(disassembled_line, ", ");
                    strcat(disassembled_line, reg_name[rx]);
                    /* prints the offset only if it's not 0. I don't need to
                     * see FOO Rd, Rx, $000 */
                    if (offset) {
                        sprintf(str_to_append, ", $%.3lx", offset);
                        strcat(disassembled_line, str_to_append);
                    }
                }
                return disassembled_line;
            case 0x00400000:
                /* base index shifted mode, output should look like "FOO Rd,
                 * Rx, Ry << shift" */
                shift = (MEMORY_SHIFT_MASK & instruction_word) >> 15 ;
                strcat(disassembled_line, ", ");
                strcat(disassembled_line, reg_name[rx]);
                strcat(disassembled_line, ", ");
                strcat(disassembled_line, reg_name[ry]);
                /* I don't need the shift amount value if it's 0 */
                if (shift) {
                    sprintf(str_to_append, " << %d", shift);
                    strcat(disassembled_line, str_to_append);
                }
                return disassembled_line;
            case 0x00600000:
                /* auto-indexing mode, output should look like "FOO Rd, Rx,
                 * Ry+" */
                strcat(disassembled_line, ", ");
                strcat(disassembled_line, reg_name[rx]);
                strcat(disassembled_line, ", ");
                switch (MEMORY_AUTO_MASK & instruction_word) {
                    case 0x0:
                        strcat(disassembled_line, reg_name[ry]);
                        strcat(disassembled_line, "+");
                        return disassembled_line;
                    case 0x00040000:
                        strcat(disassembled_line, reg_name[ry]);
                        strcat(disassembled_line, "-");
                        return disassembled_line;
                    case 0x00080000:
                        strcat(disassembled_line, "+");
                        strcat(disassembled_line, reg_name[ry]);
                        return disassembled_line;
                    case 0x000c0000:
                        strcat(disassembled_line, "-");
                        strcat(disassembled_line, reg_name[ry]);
                        return disassembled_line;
                    default: /* Should not happen */
                        fprintf(stderr, "Fatal error ! \n");
                        exit(EXIT_FAILURE);
                }
            default: /* Should not happen */
                fprintf(stderr, "Fatal error ! \n");
                exit(EXIT_FAILURE);
        }
    } else {
        /* Case when it's an ALU operation */

        /* Verify if there is a conditionnal prefix or not */
        if (extract_condition(instruction_word)) {
            for (i = 0; i < sizeof(prefix)/sizeof(prefix[0]) && !is_found; i++)
                is_found = !(extract_condition(instruction_word) ^ prefix[i].word);
            i--;
            if (!is_found) {
                strcpy(disassembled_line, "ILG");
                return disassembled_line;
            } else /* prints the corresponding prefix */
                strcat(disassembled_line, prefix[i].prefix);
        }
        /* Searches the corresponding mnemonic */
        is_found = false;
        for (i = 0; i < sizeof(mnemo)/sizeof(mnemo[0]) && !is_found; i++)
            is_found = !((ALU_MASK & instruction_word) ^ mnemo[i].word);
        i--;
        if (!is_found) {
            strcat(disassembled_line, "ILG");
            return disassembled_line;
        }
        /* prints the corresponding mnemonic */
        strcat(disassembled_line, mnemo[i].mnemo);
        /* checks for the suffix bit */
        if (is_suffix(instruction_word)) strcat(disassembled_line, "S ");
        else strcat(disassembled_line, " ");
        /* Appends the destination register if the instruction is not RTI or
         * SYS */
        if ((0x0fc00000 & instruction_word) == 0x07c00000) return disassembled_line;
        rd = extract_rd(instruction_word);
        /* Test if the instruction is BLK, if so format it like a branch
         * instruction */
        if ((ALU_MASK & instruction_word) == 0x06200000) {
            offset = (BRANCH_OFF_MASK & instruction_word) >> 3;
            /* Doesn't print "BLK PC, $abcd" but prints "BLK $absolute_address"
             * */
            if (rd == PC) {
                /* offset is sign extended for arithmetic purpose */
                if (offset & 0x8000)
                    offset |= 0xffffffffffff0000;
                sprintf(str_format, "$%%.%dlx", address_width_nibble);
                sprintf(str_to_append, str_format, (current_pc + offset + 4) &
                        address_width_mask);
                strcat(disassembled_line, str_to_append);
                return disassembled_line;
            } else {
                strcat(disassembled_line, reg_name[rd]);
                if (offset) {
                    if (rd) {
                        sprintf(str_to_append, ", $%.4lx", offset);
                        strcat(disassembled_line, str_to_append);
                    } else { /* Prints an address if the register is R0 */
                        /* offset is sign extended to have a full address */
                        if (offset & 0x8000)
                            offset |= 0xffffffffffff0000;
                        sprintf(str_format, "%%s, $%%.%dlx", address_width_nibble);
                        sprintf(disassembled_line, str_format, disassembled_line,
                                offset & address_width_mask);
                    }
                }
                return disassembled_line;
            }
        }
        strcat(disassembled_line, reg_name[rd]);
        /* Doesn't append Rx if the instruction is TRS, TSR, TRC or TCR */
        if ((0x0f000000 & instruction_word) == 0x07000000) return disassembled_line;
        rx = extract_rx(instruction_word);
        /* Doesn't write Rx if it's the same as Rd */
        if (rd != rx) {
            strcat(disassembled_line, ", ");
            strcat(disassembled_line, reg_name[rx]);
        }
        strcat(disassembled_line, ", ");
        ry = extract_ry(instruction_word);
        /* Get the addressing mode to format the output */
        if (is_immediate(instruction_word)) {
            imm_value = extract_imm(instruction_word);
            /* immediate value is sign extended to show the right decimal value */
            if (imm_value & 0x0100) imm_value = 0xffffff00 | imm_value;
            sprintf(str_to_append, "#$%.3x ; %d in decimal", imm_value & 0x1ff,
                    imm_value);
            strcat(disassembled_line, str_to_append);
            return disassembled_line;
        } else {
            shift = (ALU_SHIFT_MASK & instruction_word) >> 15;
            strcat(disassembled_line, reg_name[ry]);
            /* The shift value is useless if it's 0 */
            if (shift) {
                sprintf(str_to_append, " << %d", shift);
                strcat(disassembled_line, str_to_append);
            }
            return disassembled_line;
        }
    }
    /* Should not happen */
    fprintf(stderr, "Fatal error ! \n");
    exit(EXIT_FAILURE);
}

void display_help () {
    printf("Usage : ");
    printf("m64dasm inputfile [options]\n");
    printf("Options : \n");
    printf("-w, --adress-width {width}\n");
    printf("-s, --start-address {address}\n");
    printf("-r, --rom\n");
    printf("-e, --end-address {address}\n");
    printf("-m, --mnemonic-only\n");
    printf("-h, --help\n");
    printf("--version\n");
}

void display_version () {
    printf("m64dasm %s\n", VERSION);
}

#ifdef unit_test_of_disassemble_word
int main (int argc, char ** argv) {
    /* Small test fonction */
    uint32_t instruction_word;
    uint64_t current_address;
    char * disassembled_line;
    while (!feof(stdin)) {
        scanf("%lx %x", &current_address, &instruction_word);
        disassembled_line = disassemble_word(instruction_word, current_address);
        printf("%s\n", disassembled_line);
        free(disassembled_line);
    }
    exit(EXIT_SUCCESS);
}

#else /* unit_test_of_disassemble_word */
int main (int argc, char ** argv) {

    char input_file_name[256];
    FILE * input;
    char file_buffer[BUFSIZ];
    uint32_t * program_buffer;
    size_t program_buffer_size = BUFSIZ;
    size_t input_size = 0;

    /* determine the options added to the command */
    int opts;
    static struct option long_options[] = {
        {"mnemonic-only",  no_argument,       0, 'm'},
        {"start-address",  required_argument, 0, 's'},
        {"rom",            no_argument,       0, 'r'},
        {"end-address",    required_argument, 0, 'e'},
        {"address-width",  required_argument, 0, 'w'},
        {"help",           no_argument,       0, 'h'},
        {"version",        no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };
    int option_index = 0;
    /* The purpose of this boolean is to detect conflicting options */
    bool r_present = false;
    bool s_present = false;
    bool e_present = false;
    do {
        opts = getopt_long(argc, argv, "mrhs:e:w:", long_options,
                &option_index);

        switch (opts) {
            case 'm':
                mnemonic_only_flag = true;
                break;
            case 's':
                sscanf(optarg, "%lx", &start_address);
                s_present = true;
                break;
            case 'e':
                sscanf(optarg, "%lx", &end_address);
                force_end_flag = true;
                e_present = true;
                break;
            case 'r':
                force_end_flag = true;
                r_present = true;
                break;
            case 'w':
                sscanf(optarg, "%d", &address_width);
                address_width_nibble = (address_width + 3) / 4;
                address_width_mask = 1;
                for (int i = 0; i < address_width - 1; i++) {
                    address_width_mask <<= 1;
                    address_width_mask |= 1;
                }
                break;
            case 'h':
                display_help();
                exit(EXIT_SUCCESS);
            case 'v':
                display_version();
                exit(EXIT_SUCCESS);
            case -1:
                break; /* End of the options */
            case '?':  /* invalid option */
                break; /* Already handled by getopt */
            default:
                fprintf(stderr, "Fatal error while parsing options\n");
                exit(EXIT_FAILURE);
        }
    }

    while (opts != -1);
        if ((e_present && r_present) || (e_present && s_present) ||
                (r_present && s_present)) {
        fprintf(stderr, "Incompatible options\n");
        exit(EXIT_FAILURE);
    }

    if (optind == argc - 1) {
        strncpy(input_file_name, argv[optind], 256);
    } else if (optind == argc) {
            fprintf(stderr, "Input file is missing\n");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Can't handle more than one input file\n");
        exit(EXIT_FAILURE);
    }

    /* open the input file or set it to stdin if the file name is "-" */
    if (input_file_name[0] == '-' && !input_file_name[1]) {
        input = stdin;
    } else {
        input = fopen(input_file_name, "rb");
    }

    if (input == NULL) {
        fprintf(stderr, "Can't open the input file\n");
        exit(EXIT_FAILURE);
    }

    /* Disable line bufferring and use block bufferring instead, because line
     * buffering does not make any sense on a binary input. */
    setbuf(input, file_buffer);

    program_buffer = malloc(program_buffer_size);
    if (program_buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    /* Copy the input in program_buffer */
    do {
        if (!fread(program_buffer + input_size, sizeof(int32_t), 1, input) &&
                !feof(input)) {
            fprintf(stderr, "Read error\n");
            free(program_buffer);
            fclose(input);
            exit(EXIT_FAILURE);
        }
        if (4*input_size >= program_buffer_size - 16) {
            program_buffer_size += BUFSIZ;
            program_buffer = realloc(program_buffer, program_buffer_size);
            if (program_buffer == NULL) {
                fprintf(stderr, "Failed to allocate memory\n");
                fclose(input);
                exit(EXIT_FAILURE);
            }
        }
        input_size++;
    } while (!feof(input)) ;
    input_size--;

    if (force_end_flag) {
        start_address = end_address - 4*input_size;
    }

    char str_format[32];
    char * disassembled_line;
    for (uint64_t i = 0; i < input_size ; i++) {
        if (!mnemonic_only_flag) {
            sprintf(str_format, "%%.%dlx:    %%.8x", address_width_nibble);
            printf(str_format, (start_address + 4*i) & address_width_mask,
                    *(program_buffer + i));
        }

        disassembled_line = disassemble_word(*(program_buffer + i),
                start_address + 4*i);
        printf("    %s\n", disassembled_line);
    } 
    free(disassembled_line);

    free(program_buffer);
    fclose(input);

    exit(EXIT_SUCCESS);
}
#endif /* unit_test_of_disassemble_word */
