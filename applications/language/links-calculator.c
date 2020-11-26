#include "executor.h"
#include <expression-parser/expression-parser.h>


void calculate_links(Parser* parser)
{
	Expression new_expression;

	for_each_buffer_element(&parser->expressions, Expression, expression)
		initialize_buffer(&new_expression.expression_data, 100);
			for_each_buffer_element(&expression.expression_data, Expression_Node, node)
			{
				switch(node.type)
				{
					case OPERAND:
					{
						Operand* operand = node.data;

						switch(operand->type)
						{
							case NAME_OPERAND:
							{
								Name_Operand* name_operand = &operand->data;
								Label_Operand* label;

								label = find_label(parser, &name_operand->name);
								
								if(label)
								{
									add_expression_node(&new_expression.expression_data, OPERAND, create_label_link_operand(label));
								}
								else
								{
									Variable_Operand* variable = find_variable(parser, &name_operand->name);

									if(variable)
									{
										add_expression_node(&new_expression.expression_data, OPERAND, create_variable_link_operand(variable));
									}
									else
									{
										add_expression_node(&new_expression.expression_data, OPERAND, operand);
										/*printf("error: undefined link ");
										print_token(&name_operand->name);
										printf("\n");
										parser->has_error = 1;*/
									}
								}
								
								break;
							}

							default:
							{
								add_expression_node(&new_expression.expression_data, OPERAND, operand);
							}
						}
						break;
					}

					case UNARY_OPERATION:
					{
						add_expression_node(&new_expression.expression_data, UNARY_OPERATION, node.data);
						break;
					}

					case BINARY_OPERATION:
					{
						add_expression_node(&new_expression.expression_data, BINARY_OPERATION, node.data);
						break;
					}
				}
			}
			end_for_each_buffer_element

			add_in_buffer_end(&parser->result_expressions, Expression, new_expression);
	end_for_each_buffer_element
}