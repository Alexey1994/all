#include "x86.h"
#include <system/memory.h>


static void write_variable()
{

}


static void write_address(Generator* generator, Byte byte)
{
	++generator->current_address;
}


static void write_in_file(Generator* generator, Byte byte)
{
	generator->write_byte(generator->source, byte);
	++generator->current_address;
}


typedef struct
{
	N_32   type;
	N_32   address;
	Buffer name;
	N_32   type2;
}
Code_Label;


void get_code_labels(Buffer* expressions, Buffer* labels)
{
	for_each_buffer_element(expressions, Buffer, expression)
		for_each_buffer_element(&expression, Expression_Node, node)
			switch(node.type)
			{
				case OPERAND:
				{
					Operand_Node* operand = node.data;

					switch(operand->type)
					{
						case LABEL:
						{
							Label_Operand* label_operand = operand;
							Code_Label* label;
							label = allocate_memory(sizeof(Code_Label));
							label->type = label->type2 = LABEL_LINK;
							
							initialize_buffer(&label->name, 10);
							add_buffer_in_buffer_end(&label->name, &label_operand->name);
							
							label->address = 0;
							add_in_buffer_end(labels, Code_Label*, label);
							break;
						}
					}
					break;
				}
			}
		end_for_each_buffer_element
	end_for_each_buffer_element
}


Code_Label* find_code_label(Buffer* labels, Buffer* name)
{
	for_each_buffer_element(labels, Code_Label*, label)
		if(!compare_UTF8_string(&label->name, name))
			return label;
	end_for_each_buffer_element

	return 0;
}


typedef struct
{
	N_32   type;
	N_32   address;
	Buffer name;
	N_32   offset;
	N_32   variable_type;
	N_32   type2;
}
Variable;


Variable* find_variable(Generator* generator, Buffer* variable_name)
{
	for_each_buffer_element(&generator->variables, Variable*, variable)
		if(!compare_UTF8_string(&variable->name, variable_name))
			return variable;
	end_for_each_buffer_element

	return 0;
}


void add_variable(Generator* generator, Buffer* variable_name, N_32 variable_type)
{
	Variable* variable;
	variable = allocate_memory(sizeof(Variable));

	variable->type = variable->type2 = VARIABLE;

	initialize_buffer(&variable->name, 10);
	add_buffer_in_buffer_end(&variable->name, variable_name);
	
	variable->offset = generator->current_address;
	add_in_buffer_end(&generator->variables, Variable*, variable);

	switch(variable_type)
	{
		case CAST_TO_NUMBER8:  generator->current_address += 8; break;
		case CAST_TO_NUMBER16: generator->current_address += 16; break;
		case CAST_TO_NUMBER32: generator->current_address += 32; break;
		default:
			printf("undefined variable type in add_variable (type is %d)\n", variable_type);
	}
}


Operand_Node* get_operand_from_stack(Buffer* operands_stack)
{
	if(!operands_stack->length)
	{
		printf("error: not found next operand\n");
		return 0;
	}

	switch(operands_stack->data[operands_stack->length - sizeof(N_32)])
	{
		case LABEL_LINK:           operands_stack->length -= sizeof(Code_Label); break;
		case NUMBER:               operands_stack->length -= sizeof(Number_Operand); break;
		case VARIABLE:             operands_stack->length -= sizeof(Variable_Operand); break;
		case POINTER_FROM_STACK:   operands_stack->length -= sizeof(Pointer_From_Stack); break;

		case CAST_TO_NUMBER8:      operands_stack->length -= sizeof(Cast_To_Number8); break;
		case CAST_TO_NUMBER16:     operands_stack->length -= sizeof(Cast_To_Number16); break;
		case CAST_TO_NUMBER32:     operands_stack->length -= sizeof(Cast_To_Number32); break;

		case NUMBER8_FROM_STACK:   operands_stack->length -= sizeof(Number8_From_Stack); break;
		case NUMBER16_FROM_STACK:  operands_stack->length -= sizeof(Number16_From_Stack); break;
		case NUMBER32_FROM_STACK:  operands_stack->length -= sizeof(Number32_From_Stack); break;

		case UNDEFINED_FROM_STACK: operands_stack->length -= sizeof(Undefined_From_Stack); break;

		default:
			printf("undefined operand size in get_operand_from_stack: %d\n", operands_stack->data[operands_stack->length]);
			return 0;
	}

	return operands_stack->data + operands_stack->length;
}


N_32 write_operand(Operand_Node* operand, void (*write_byte)(Generator* generator, Byte byte), Generator* generator, Buffer* operands_stack)
{
	N_32 operand_size;

	switch(operand->type)
	{
		case LABEL_LINK:
		{
			Label_Operand* target_operand = operand;
			operand_size = 32;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0xB8);
			cycle(0, 4, 1)
				write_byte(generator, ((N_8*)&target_operand->address)[i]);
			end

			#ifdef DEV_MODE
			printf("MOV EAX, %d\n", target_operand->address);
			#endif
			break;
		}

		case NUMBER:
		{
			Number_Operand* target_operand = operand;
			operand_size = 32;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0xB8);
			cycle(0, 4, 1)
				write_byte(generator, ((N_8*)&target_operand->value)[i]);
			end

			#ifdef DEV_MODE
			printf("MOV EAX, %d\n", target_operand->value);
			#endif
			break;
		}

		case VARIABLE:
		{
			Variable_Operand* target_operand = operand;
			operand_size = 32;

			if(write_byte == write_variable)
				break;

			Variable* target_variable = find_variable(generator, &target_operand->name);

			if(!target_variable)
			{
				printf("variable ");
				print_token(&target_operand->name);
				printf(" not defined\n");
				break;
			}

			switch(target_variable->variable_type)
			{
				case CAST_TO_NUMBER8:
				{
					operand_size = 8;

					write_byte(generator, 0x8A);
					write_byte(generator, 0x85);
					cycle(0, 4, 1)
						write_byte(generator, ((N_8*)&target_operand->address)[i]);
					end

					#ifdef DEV_MODE
					printf("MOV AL, [EBP - %d]\n", target_variable->offset);
					#endif
					break;
				}

				case CAST_TO_NUMBER16:
				{
					operand_size = 16;

					write_byte(generator, 0x66);
					write_byte(generator, 0x8B);
					write_byte(generator, 0x85);
					cycle(0, 4, 1)
						write_byte(generator, ((N_8*)&target_operand->address)[i]);
					end

					#ifdef DEV_MODE
					printf("MOV AX, [EBP - %d]\n", target_variable->offset);
					#endif
					break;
				}

				case CAST_TO_NUMBER32:
				{
					operand_size = 32;

					write_byte(generator, 0x8B);
					write_byte(generator, 0x85);
					cycle(0, 4, 1)
						write_byte(generator, ((N_8*)&target_operand->address)[i]);
					end

					#ifdef DEV_MODE
					printf("MOV EAX, [EBP - %d]\n", target_variable->offset);
					#endif
					break;
				}
			}

			break;
		}

		case POINTER_FROM_STACK:
		{
			operand_size = 32;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0x5B);

			write_byte(generator, 0x8B);
			write_byte(generator, 0x03);

			#ifdef DEV_MODE
			printf("POP EBX\n");
			printf("MOV EAX, [EBX]\n");
			#endif
			break;
		}

		case CAST_TO_NUMBER8:
		{
			operand_size = 8;
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			N_32 casted_operand_size = write_operand(casted_operand, write_byte, generator, operands_stack);
			break;
		}

		case CAST_TO_NUMBER16:
		{
			operand_size = 16;
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			N_32 casted_operand_size = write_operand(casted_operand, write_byte, generator, operands_stack);

			switch(casted_operand_size)
			{
				case 8:
				{
					write_byte(generator, 0x66);
					write_byte(generator, 0x0F);
					write_byte(generator, 0xB6);
					write_byte(generator, 0xC0);

					#ifdef DEV_MODE
					printf("MOVZX AX, AL\n");
					#endif
					break;
				}

				case 16:
				{
					break;
				}

				case 32:
				{
					break;
				}
			}

			break;
		}

		case CAST_TO_NUMBER32:
		{
			operand_size = 32;
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			N_32 casted_operand_size = write_operand(casted_operand, write_byte, generator, operands_stack);

			switch(casted_operand_size)
			{
				case 8:
				{
					write_byte(generator, 0x0F);
					write_byte(generator, 0xB6);
					write_byte(generator, 0xC0);

					#ifdef DEV_MODE
					printf("MOVZX EAX, AL\n");
					#endif
					break;
				}

				case 16:
				{
					write_byte(generator, 0x0F);
					write_byte(generator, 0xB7);
					write_byte(generator, 0xC0);

					#ifdef DEV_MODE
					printf("MOVZX EAX, AX\n");
					#endif
					break;
				}

				case 32:
				{
					break;
				}
			}

			break;
		}

		case NUMBER8_FROM_STACK:
		{
			operand_size = 8;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0x8A);
			write_byte(generator, 0x04);
			write_byte(generator, 0x24);

			write_byte(generator, 0x83);
			write_byte(generator, 0xC4);
			write_byte(generator, 0x01);

			#ifdef DEV_MODE
			printf("MOV AL, [ESP]\n");
			printf("ADD ESP, 1\n");
			#endif
			break;
		}

		case NUMBER16_FROM_STACK:
		{
			operand_size = 16;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0x66);
			write_byte(generator, 0x58);

			#ifdef DEV_MODE
			printf("POP AX\n");
			#endif
			break;
		}

		case NUMBER32_FROM_STACK:
		{
			operand_size = 32;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0x58);

			#ifdef DEV_MODE
			printf("POP EAX\n");
			#endif
			break;
		}

		default:
			printf("undefined operand type: %d\n", operand->type);
	}

	return operand_size;
}


void write_operand_address(Operand_Node* operand, void (*write_byte)(Generator* generator, Byte byte), Generator* generator, Buffer* operands_stack)
{
	switch(operand->type)
	{
		case LABEL_LINK:
		{
			Label_Operand* target_operand = operand;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0xB8);
			cycle(0, 4, 1)
				write_byte(generator, ((N_8*)&target_operand->address)[i]);
			end

			#ifdef DEV_MODE
			printf("MOV EAX, %d\n", target_operand->address);
			#endif
			break;
		}

		case NUMBER:
		{
			Number_Operand* target_operand = operand;

			if(write_byte == write_variable)
				break;

			write_byte(generator, 0xB8);
			cycle(0, 4, 1)
				write_byte(generator, ((N_8*)&target_operand->value)[i]);
			end

			#ifdef DEV_MODE
			printf("MOV EAX, %d\n", target_operand->value);
			#endif
			break;
		}

		case VARIABLE:
		{
			Variable_Operand* target_operand = operand;

			if(write_byte == write_variable)
				break;

			Variable* target_variable = find_variable(generator, &target_operand->name);

			if(!target_variable)
			{
				printf("variable ");
				print_token(&target_operand->name);
				printf(" not defined\n");
				break;
			}

			write_byte(generator, 0x89);
			write_byte(generator, 0xE8);

			write_byte(generator, 0x2D);
			cycle(0, 4, 1)
				write_byte(generator, ((N_8*)&target_variable->offset)[i]);
			end

			#ifdef DEV_MODE
			printf("MOV EAX, EBP\n");
			printf("SUB EAX, %d\n", target_variable->offset);
			#endif
			break;
		}

		case POINTER_FROM_STACK:
		{
			if(write_byte == write_variable)
				break;

			write_byte(generator, 0x5B);

			write_byte(generator, 0x8B);
			write_byte(generator, 0x03);

			#ifdef DEV_MODE
			printf("POP EBX\n");
			printf("MOV EAX, [EBX]\n");
			#endif
			break;
		}

		case CAST_TO_NUMBER8:
		{
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			write_operand_address(casted_operand, write_byte, generator, operands_stack);
			break;
		}

		case CAST_TO_NUMBER16:
		{
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			write_operand_address(casted_operand, write_byte, generator, operands_stack);
			break;
		}

		case CAST_TO_NUMBER32:
		{
			Operand_Node* casted_operand = get_operand_from_stack(operands_stack);
			write_operand_address(casted_operand, write_byte, generator, operands_stack);
			break;
		}

		case NUMBER8_FROM_STACK:
		{
			#ifdef DEV_MODE
			printf("error getting address from stack\n");
			#endif
			break;
		}

		case NUMBER16_FROM_STACK:
		{
			#ifdef DEV_MODE
			printf("error getting address from stack\n");
			#endif
			break;
		}

		case NUMBER32_FROM_STACK:
		{
			#ifdef DEV_MODE
			printf("error getting address from stack\n");
			#endif
			break;
		}

		default:
			printf("undefined operand type: %d\n", operand->type);
	}
}


void create_code_sequence(Buffer* expressions, Buffer* labels, void (*write_byte)(Generator* generator, Byte byte), Generator* generator)
{
	for_each_buffer_element(expressions, Buffer, expression)
		BUFFER(operands_stack, 4)
			for_each_buffer_element(&expression, Expression_Node, node)
			{
				switch(node.type)
				{
					case OPERAND:
					{
						Operand_Node* operand = node.data;

						switch(operand->type)
						{
							case LABEL:
							{
								Label_Operand* label_operand = operand;
								
								if(write_byte == write_address)
								{
									Code_Label* label = find_code_label(labels, &label_operand->name);
									label_operand->address = label->address = generator->current_address;
								}

								#if defined(DEV_MODE)
								printf("; ");
								print_token(&label_operand->name);
								printf(" %d\n", label_operand->address);
								#endif

								break;
							}

							case VARIABLE:
							{
								Variable_Operand* variable_operand = operand;
								Code_Label* label_place = find_code_label(labels, &variable_operand->name);

								if(label_place)
								{
									add_in_buffer_end(&operands_stack, Code_Label, *label_place);
								}
								else
								{
									add_in_buffer_end(&operands_stack, Variable_Operand, *variable_operand);
								}
								break;
							}

							case NUMBER:
							{
								Number_Operand* number_operand = operand;
								add_in_buffer_end(&operands_stack, Number_Operand, *number_operand);
								break;
							}

							case STATIC_STRING:
							{
								Static_String_Operand* static_string_operand = operand;
								add_in_buffer_end(&operands_stack, Static_String_Operand, *static_string_operand);
								break;
							}

							case CALL:
							{
								Call_Operand* call_operand = operand;
								add_in_buffer_end(&operands_stack, Call_Operand, *call_operand);
								break;
							}

							default:
								printf("undefined operand type: %d\n", operand->type);
						}
						break;
					}

					case UNARY_OPERATION:
						if(node.data == 'N')
						{
							Cast_To_Number32 result = {.type = CAST_TO_NUMBER32, .type2 = CAST_TO_NUMBER32};
							add_in_buffer_end(&operands_stack, Cast_To_Number32, result);
						}
						else if(node.data == 'p')
						{
							#ifdef DEV_MODE
							printf("CALL printf\n");
							#endif

							Undefined_From_Stack result = {.type = UNDEFINED_FROM_STACK, .type2 = UNDEFINED_FROM_STACK};
							add_in_buffer_end(&operands_stack, Undefined_From_Stack, result);
						}
						else
						{
							//temporary
							Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
							add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
						}
						break;

					case BINARY_OPERATION:
					{
						Operand_Node* right_operand = get_operand_from_stack(&operands_stack);
						Operand_Node* left_operand = get_operand_from_stack(&operands_stack);

						if(node.data == '=')
						{
								write_operand(right_operand, write_byte, generator, &operands_stack);

								write_byte(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand_address(left_operand, write_byte, generator, &operands_stack);

 								write_byte(generator, 0x5B);

 								write_byte(generator, 0x89);
 								write_byte(generator, 0x18);

 								write_byte(generator, 0x53);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("MOV [EAX], EBX\n");
								printf("PUSH EBX\n");
								#endif

								Undefined_From_Stack result = {.type = UNDEFINED_FROM_STACK, .type2 = UNDEFINED_FROM_STACK};
								add_in_buffer_end(&operands_stack, Undefined_From_Stack, result);
						}
						else if(node.data == '+')
						{
							write_operand(right_operand, write_byte, generator, &operands_stack);

							write_byte(generator, 0x50);
							#ifdef DEV_MODE
							printf("PUSH EAX\n");
							#endif

							write_operand(left_operand, write_byte, generator, &operands_stack);

							write_byte(generator, 0x5B);

							write_byte(generator, 0x01);
							write_byte(generator, 0xD8);

							write_byte(generator, 0x50);
								
							#ifdef DEV_MODE
							printf("POP EBX\n");
							printf("ADD EAX, EBX\n");
							printf("PUSH EAX\n");
							#endif

							Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
							add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
						}
						break;
					}
/*
					case OPERATION:
						if(((Operation*)node.data)->type == UNARY_OPERATION)
						{
							if(!strcmp(((Operation*)node.data)->name, "Number"))
							{
								Cast_To_Number32 result = {.type = CAST_TO_NUMBER32, .type2 = CAST_TO_NUMBER32};
								add_in_buffer_end(&operands_stack, Cast_To_Number32, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "Number8"))
							{
								Cast_To_Number8 result = {.type = CAST_TO_NUMBER8, .type2 = CAST_TO_NUMBER8};
								add_in_buffer_end(&operands_stack, Cast_To_Number8, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "Number16"))
							{
								Cast_To_Number16 result = {.type = CAST_TO_NUMBER16, .type2 = CAST_TO_NUMBER16};
								add_in_buffer_end(&operands_stack, Cast_To_Number16, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "Number32"))
							{
								Cast_To_Number32 result = {.type = CAST_TO_NUMBER32, .type2 = CAST_TO_NUMBER32};
								add_in_buffer_end(&operands_stack, Cast_To_Number32, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "*"))
							{
								Pointer_From_Stack result = {.type = POINTER_FROM_STACK, .type2 = POINTER_FROM_STACK};
								add_in_buffer_end(&operands_stack, Pointer_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "&"))
							{
								Operand_Node* operand = get_operand_from_stack(&operands_stack);
								write_operand_address(operand, write, generator, &operands_stack);

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif
							}
							else
							{
								Operand_Node* operand = get_operand_from_stack(&operands_stack);
								N_32 operand_size = write_operand(operand, write, generator, &operands_stack);

								if(!strcmp(((Operation*)node.data)->name, "!"))
								{
									if(operand_size == 8)
									{
										write(generator, 0x3C);
										write(generator, 0x00);

										write(generator, 0x74);
										write(generator, 0x04);

										write(generator, 0x6A);
										write(generator, 0x01);

										write(generator, 0xEB);
										write(generator, 0x02);

										write(generator, 0x6A);
										write(generator, 0x00);

										#ifdef DEV_MODE
										printf("CMP AL, 0\n");
										printf("JZ a\n");
										printf("PUSH byte 1\n");
										printf("JUMP b\n");
										printf("a:\n");
										printf("PUSH byte 0\n");
										printf("b:\n");
										#endif
									}
									else if(operand_size == 16)
									{
										write(generator, 0x66);
										write(generator, 0x83);
										write(generator, 0xF8);
										write(generator, 0x00);

										write(generator, 0x74);
										write(generator, 0x04);

										write(generator, 0x6A);
										write(generator, 0x01);

										write(generator, 0xEB);
										write(generator, 0x02);

										write(generator, 0x6A);
										write(generator, 0x00);

										#ifdef DEV_MODE
										printf("CMP AX, 0\n");
										printf("JZ a\n");
										printf("PUSH byte 1\n");
										printf("JUMP b\n");
										printf("a:\n");
										printf("PUSH byte 0\n");
										printf("b:\n");
										#endif
									}
									else if(operand_size == 32)
									{
										write(generator, 0x83);
										write(generator, 0xF8);
										write(generator, 0x00);

										write(generator, 0x74);
										write(generator, 0x04);

										write(generator, 0x6A);
										write(generator, 0x01);

										write(generator, 0xEB);
										write(generator, 0x02);

										write(generator, 0x6A);
										write(generator, 0x00);

										#ifdef DEV_MODE
										printf("CMP EAX, 0\n");
										printf("JZ a\n");
										printf("PUSH byte 1\n");
										printf("JUMP b\n");
										printf("a:\n");
										printf("PUSH byte 0\n");
										printf("b:\n");
										#endif
									}

									Number8_From_Stack result = {.type = NUMBER8_FROM_STACK, .type2 = NUMBER8_FROM_STACK};
									add_in_buffer_end(&operands_stack, Number8_From_Stack, result);
								}
								else if(!strcmp(((Operation*)node.data)->name, "-"))
								{
									if(operand_size == 8)
									{
										write(generator, 0x89);
										write(generator, 0xC3);

										write(generator, 0xB8);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);

										write(generator, 0x29);
										write(generator, 0xD8);

										write(generator, 0x88);
										write(generator, 0x04);
										write(generator, 0x24);

										write(generator, 0x83);
										write(generator, 0xEC);
										write(generator, 0x01);

										#ifdef DEV_MODE
										printf("MOV EBX, EAX\n");
										printf("MOV EAX, 0\n");
										printf("SUB EAX, EBX\n");

										printf("MOV [ESP], AL\n");
										printf("SUB ESP, 1\n");
										#endif

										Number8_From_Stack result = {.type = NUMBER8_FROM_STACK, .type2 = NUMBER8_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number8_From_Stack, result);
									}
									else if(operand_size == 16)
									{
										write(generator, 0x89);
										write(generator, 0xC3);

										write(generator, 0xB8);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);

										write(generator, 0x29);
										write(generator, 0xD8);

										write(generator, 0x66);
										write(generator, 0x50);

										#ifdef DEV_MODE
										printf("MOV EBX, EAX\n");
										printf("MOV EAX, 0\n");
										printf("SUB EAX, EBX\n");
										printf("PUSH AX\n");
										#endif

										Number16_From_Stack result = {.type = NUMBER16_FROM_STACK, .type2 = NUMBER16_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number16_From_Stack, result);
									}
									else if(operand_size == 32)
									{
										write(generator, 0x89);
										write(generator, 0xC3);

										write(generator, 0xB8);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);
										write(generator, 0x00);

										write(generator, 0x29);
										write(generator, 0xD8);

										write(generator, 0x50);

										#ifdef DEV_MODE
										printf("MOV EBX, EAX\n");
										printf("MOV EAX, 0\n");
										printf("SUB EAX, EBX\n");
										printf("PUSH EAX\n");
										#endif

										Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
									}
								}
								else if(!strcmp(((Operation*)node.data)->name, "~"))
								{
									if(operand_size == 8)
									{
										write(generator, 0xF6);
										write(generator, 0xD0);

										write(generator, 0x88);
										write(generator, 0x04);
										write(generator, 0x24);

										write(generator, 0x83);
										write(generator, 0xEC);
										write(generator, 0x01);

										#ifdef DEV_MODE
										printf("NOT AL\n");
										printf("PUSH AL\n");
										#endif

										Number8_From_Stack result = {.type = NUMBER8_FROM_STACK, .type2 = NUMBER8_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number8_From_Stack, result);
									}
									else if(operand_size == 16)
									{
										write(generator, 0x66);
										write(generator, 0xF7);
										write(generator, 0xD0);

										write(generator, 0x66);
										write(generator, 0x50);

										#ifdef DEV_MODE
										printf("NOT AX\n");
										printf("PUSH AX\n");
										#endif

										Number16_From_Stack result = {.type = NUMBER16_FROM_STACK, .type2 = NUMBER16_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number16_From_Stack, result);
									}
									else if(operand_size == 32)
									{
										write(generator, 0xF7);
										write(generator, 0xD0);

										write(generator, 0x50);

										#ifdef DEV_MODE
										printf("NOT EAX\n");
										printf("PUSH EAX\n");
										#endif

										Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
										add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
									}
								}
								else
								{
									Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
									add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
								}
							}
						}
						else if(((Operation*)node.data)->type == BINARY_OPERATION)
						{
							Operand_Node* right_operand = get_operand_from_stack(&operands_stack);
							Operand_Node* left_operand = get_operand_from_stack(&operands_stack);

							if(!strcmp(((Operation*)node.data)->name, "if"))
							{
								write_operand(right_operand, write, generator, &operands_stack);
								
								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand_address(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x83);
								write(generator, 0xFB);
								write(generator, 0x00);

								write(generator, 0x75);
								write(generator, 0x02);

								write(generator, 0xFF);
								write(generator, 0xE0);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("CMP EBX, 0\n");
								printf("JNE a\n");
								printf("JMP EAX\n");
								printf("a:\n");
								#endif

								Undefined_From_Stack result = {.type = UNDEFINED_FROM_STACK, .type2 = UNDEFINED_FROM_STACK};
								add_in_buffer_end(&operands_stack, Undefined_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "&&"))
							{
								//?
								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "||"))
							{
								//?
								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "&"))
							{
								write_operand(right_operand, write, generator, &operands_stack);
								
								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x21);
								write(generator, 0xD8);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("AND EAX, EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "|"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x09);
								write(generator, 0xD8);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("OR EAX, EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "^"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x31);
								write(generator, 0xD8);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("XOR EAX, EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "<<"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0xD3);
								write(generator, 0xE0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("SHL EAX, CL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, ">>"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);
								
								write(generator, 0x59);

								write(generator, 0xD3);
								write(generator, 0xE8);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("SHR EAX, CL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "=="))
							{								
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x94);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETZ AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "!="))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x95);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETNZ AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "<"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x92);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETC AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "<="))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x96);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETNA AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, ">"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x97);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETA AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, ">="))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x59);

								write(generator, 0x39);
								write(generator, 0xD8);

								write(generator, 0xB8);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);
								write(generator, 0x00);

								write(generator, 0x0F);
								write(generator, 0x93);
								write(generator, 0xC0);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP ECX\n");
								printf("CMP EAX, EBX\n");
								printf("MOV EAX, 0\n");
								printf("SETNC AL\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "="))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand_address(left_operand, write, generator, &operands_stack);

 								write(generator, 0x5B);

 								write(generator, 0x89);
 								write(generator, 0x18);

 								write(generator, 0x53);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("MOV [EAX], EBX\n");
								printf("PUSH EBX\n");
								#endif

								Undefined_From_Stack result = {.type = UNDEFINED_FROM_STACK, .type2 = UNDEFINED_FROM_STACK};
								add_in_buffer_end(&operands_stack, Undefined_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "*"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0xF7);
								write(generator, 0xE3);

								write(generator, 0x50);

								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("MUL EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "/"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0xF7);
								write(generator, 0xF3);

								write(generator, 0x50);
								
								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("DIV EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "%"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0xF7);
								write(generator, 0xF3);

								write(generator, 0x52);
								
								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("DIV EBX\n");
								printf("PUSH EDX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "+"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x01);
								write(generator, 0xD8);

								write(generator, 0x50);
								
								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("ADD EAX, EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else if(!strcmp(((Operation*)node.data)->name, "-"))
							{
								write_operand(right_operand, write, generator, &operands_stack);

								write(generator, 0x50);
								#ifdef DEV_MODE
								printf("PUSH EAX\n");
								#endif

								write_operand(left_operand, write, generator, &operands_stack);

								write(generator, 0x5B);

								write(generator, 0x29);
								write(generator, 0xD8);

								write(generator, 0x50);
								
								#ifdef DEV_MODE
								printf("POP EBX\n");
								printf("SUB EAX, EBX\n");
								printf("PUSH EAX\n");
								#endif

								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
							else
							{
								Number32_From_Stack result = {.type = NUMBER32_FROM_STACK, .type2 = NUMBER32_FROM_STACK};
								add_in_buffer_end(&operands_stack, Number32_From_Stack, result);
							}
						}
						else
						{
							printf("broken data in operation\n");
						}
*/
						break;

					default:
						printf("broken data in expression\n");
				}
			}
			end_for_each_buffer_element

			if(operands_stack.length)
			{
				switch(operands_stack.data[operands_stack.length - sizeof(N_32)])
				{
					case NUMBER8_FROM_STACK:
					case NUMBER16_FROM_STACK:
					case NUMBER32_FROM_STACK:
					{
						break;
					}

					case NUMBER:
					{
						Number_Operand* number_operand = operands_stack.data + (operands_stack.length - sizeof(Number_Operand));
						write_byte(generator, number_operand->value);
						printf("%d", number_operand->value);
						break;
					}

					case LABEL_LINK:
					{
						Code_Label* label = operands_stack.data + (operands_stack.length - sizeof(Code_Label));
						Code_Label* target_label = find_code_label(labels, &label->name);

						N_32 current_address = generator->current_address + 5;
						N_32 address_node = label->address - current_address;
						write_byte(generator, 0xE9);
						cycle(0, 4, 1)	
							write_byte(generator, ((N_8*)&address_node)[i]);
						end

						#if defined(DEV_MODE)
						printf("JMP ");
						print_token(&label->name);
						printf("\n");
						#endif
						break;
					}

					case UNDEFINED_FROM_STACK:
					{
						break;
					}

					case CAST_TO_NUMBER8:
					{
						Cast_To_Number8* number8_type_operand = get_operand_from_stack(&operands_stack);

						switch(operands_stack.data[operands_stack.length - sizeof(N_32)])
						{
							case VARIABLE:
							{
								if(write_byte != write_variable)
									break;

								Variable_Operand* variable = operands_stack.data + (operands_stack.length - sizeof(Variable_Operand));

								if(find_variable(generator, &variable->name))
								{
									printf("error: variable ");
									print_token(&variable->name);
									printf(" was defined\n");
								}
								else
								{
									add_variable(generator, &variable->name, CAST_TO_NUMBER8);
									Variable* target_variable = find_variable(generator, &variable->name);
									target_variable->variable_type = CAST_TO_NUMBER8;
								}

								#if defined(DEV_MODE)
								printf("Number8 ");
								print_token(&variable->name);
								printf("\n");
								#endif
								break;
							}

							default:
								printf("undefined type cast to Number8 for operand %d\n", operands_stack.data[operands_stack.length - sizeof(N_32)]);
						}
						break;
					}

					case CAST_TO_NUMBER16:
					{
						Cast_To_Number16* number16_type_operand = get_operand_from_stack(&operands_stack);

						switch(operands_stack.data[operands_stack.length - sizeof(N_32)])
						{
							case VARIABLE:
							{
								if(write_byte != write_variable)
									break;

								Variable_Operand* variable = operands_stack.data + (operands_stack.length - sizeof(Variable_Operand));

								if(find_variable(generator, &variable->name))
								{
									printf("error: variable ");
									print_token(&variable->name);
									printf(" was defined\n");
								}
								else
								{
									add_variable(generator, &variable->name, CAST_TO_NUMBER16);
									Variable* target_variable = find_variable(generator, &variable->name);
									target_variable->variable_type = CAST_TO_NUMBER16;
								}

								#if defined(DEV_MODE)
								printf("Number16 ");
								print_token(&variable->name);
								printf("\n");
								#endif
								break;
							}

							default:
								printf("undefined type cast to Number16 for operand %d\n", operands_stack.data[operands_stack.length - sizeof(N_32)]);
						}
						break;
					}

					case CAST_TO_NUMBER32:
					{
						Cast_To_Number32* number32_type_operand = get_operand_from_stack(&operands_stack);

						switch(operands_stack.data[operands_stack.length - sizeof(N_32)])
						{
							case VARIABLE:
							{
								if(write_byte != write_variable)
									break;

								Variable_Operand* variable = operands_stack.data + (operands_stack.length - sizeof(Variable_Operand));

								if(find_variable(generator, &variable->name))
								{
									printf("error: variable ");
									print_token(&variable->name);
									printf(" was defined\n");
								}
								else
								{
									add_variable(generator, &variable->name, CAST_TO_NUMBER32);
									Variable* target_variable = find_variable(generator, &variable->name);
									target_variable->variable_type = CAST_TO_NUMBER32;
								}

								#if defined(DEV_MODE)
								printf("Number32 ");
								print_token(&variable->name);
								printf("\n");
								#endif
								break;
							}

							default:
								printf("undefined type cast to Number32 for operand %d\n", operands_stack.data[operands_stack.length - sizeof(N_32)]);
						}
						break;
					}

					default:
						printf("undefined code writer type %d\n", operands_stack.data[operands_stack.length - sizeof(N_32)]);
				}
			}
		END_BUFFER(operands_stack)
	end_for_each_buffer_element
}


void generate_x86_code(Buffer* expressions, Byte* source, void (*write_byte)(Byte* source, Byte byte))
{
	Generator generator;
	initialize_buffer(&generator.variables, 20);
	generator.local_variables = 0;
	generator.current_address = 0;
	generator.source = source;
	generator.write_byte = write_byte;

	BUFFER(labels, 100)
	{
		get_code_labels(expressions, &labels);
		generator.current_address = 0;
		#ifdef DEV_MODE
		printf("\n\n############################# Generate variables #######################\n\n");
		#endif
		create_code_sequence(expressions, &labels, &write_variable, &generator);

		generator.current_address = 0;
		#ifdef DEV_MODE
		printf("\n\n############################# Generate addresses #######################\n\n");
		#endif
		create_code_sequence(expressions, &labels, &write_address, &generator);

		generator.current_address = 0;
		#ifdef DEV_MODE
		printf("\n\n############################# Generate code ############################\n\n");
		#endif
		create_code_sequence(expressions, &labels, &write_in_file, &generator);

		deinitialize_buffer(&generator.variables);
	}
	END_BUFFER(labels)
}