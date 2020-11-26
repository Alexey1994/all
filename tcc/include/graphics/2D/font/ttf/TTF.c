#include <file/file.h>

//#include <libs/graphics/graphics.h>
//#include <GL/gl.h>

#define POINT_ON_CURVE 0b00000001
#define SHORT_X        0b00000010
#define SHORT_Y        0b00000100
#define REPEAT_FLAG    0b00001000
#define X_IS_SAME      0b00010000
#define Y_IS_SAME      0b00100000
#define X_IS_POSITIVE  0b00010000
#define Y_IS_POSITIVE  0b00100000

/*
static short read_number_of_contours(Input *input)
{
    short num;

    Byte *iterator = &num;

    iterator[1] = input->head;
    read_byte(input);
    iterator[0] = input->head;

    if(num)
        read_byte(input);
    else
        return read_binary_Z_16(input);//get_bin_stream_short(in);

    return num;
}


static Vector_Glyph* read_TTF_glyph(Input *input)
{
    Vector_Glyph *glyph;
    Array        *curves;
    N_32          i;
    N_32          j;
    Z_16          number_of_contours;

    Z_16          number_of_points;

    N_16         *last_point_numbers;
    N_16          instruction_length;
    Byte         *flags;
    Z_16         *coords;

    Z_16          previouse_x;
    Z_16          previouse_y;
    Z_16          previouse_real_x;
    Z_16          previouse_real_y;

    N_32          current_contour;

    number_of_contours = read_number_of_contours(input);

    if(!number_of_contours)
        return 0;

    glyph = new(Vector_Glyph);

    glyph->min_x = read_binary_N_16(input);
    glyph->min_y = read_binary_N_16(input);
    glyph->max_x = read_binary_N_16(input);
    glyph->max_y = read_binary_N_16(input);

    if(number_of_contours < 0)
    {
        printf("this is tailed glyph\n");
        return 0;
    }

    last_point_numbers = new_array(N_16, number_of_contours);

    for(i=0; i<number_of_contours; ++i)
        last_point_numbers[i] = read_binary_N_16(input);

    number_of_points = last_point_numbers[ number_of_contours-1 ] + 1;

    //skip instructions
    instruction_length = read_binary_N_16(input);

    for(i=0; i<instruction_length; ++i)
        read_byte(input);

    //read flags
    flags = new_array(Byte, number_of_points);

    for(i=0; i<number_of_points; ++i)
    {
        Byte flag = input->head;

        if(flag & REPEAT_FLAG)
        {
            read_byte(input);
            int flag_count = input->head;

            if(i + flag_count >= number_of_points)
            {
                return 0;
            }

            for(j=0; j<flag_count && i<number_of_points; ++j, ++i)
                flags[i] = flag;
        }

        flags[i] = flag;
        read_byte(input);
    }

    int contours_count = 0;

    previouse_x      = 0;
    previouse_y      = 0;
    previouse_real_x = 0;
    previouse_real_y = 0;

    coords = new_array(Z_16, 2*number_of_points);

    //read x coords

    for(i=0; i<number_of_points; ++i)
    {
        if(flags[i] & SHORT_X)
        {
            if(flags[i] & X_IS_POSITIVE)
            {
                previouse_x = input->head;
                previouse_real_x += previouse_x;
                coords[i*2] = previouse_real_x;
            }
            else
            {
                previouse_x = -input->head;
                previouse_real_x += previouse_x;
                coords[i*2] = previouse_real_x;
            }

            read_byte(input);
        }
        else if(flags[i] & X_IS_SAME)
            coords[i*2] = previouse_real_x;
        else
        {
            previouse_x = read_binary_Z_16(input);
            previouse_real_x += previouse_x;
            coords[i*2] = previouse_real_x;
        }
    }

    //read y coords

    for(i=0; i<number_of_points; ++i)
    {
        if(flags[i] & SHORT_Y)
        {
            if(flags[i] & Y_IS_POSITIVE)
            {
                previouse_y = input->head;
                previouse_real_y += previouse_y;
                coords[i*2+1] = previouse_real_y;
            }
            else
            {
                previouse_y = -input->head;
                previouse_real_y += previouse_y;
                coords[i*2+1] = previouse_real_y;
            }

            read_byte(input);
        }
        else if(flags[i] & Y_IS_SAME)
        {
            coords[i*2+1] = previouse_real_y;
        }
        else
        {
            previouse_y = read_binary_Z_16(input);
            previouse_real_y += previouse_y;
            coords[i*2+1] = previouse_real_y;
        }
    }

    //build curves
    Array *curve_points;

    current_contour   = 0;
    int current_curve = 0;
    int points_count  = 0;

    Curve *first_contour_curve;
    Curve *last_contour_curve;

    Point *point;

    curves = create_array(2, 0);//curves = create_array(2, &destroy_curve);
    first_contour_curve = 0;

    for(i=0; i<number_of_points; ++i)
    {
        Byte flag = flags[i];

        if(i >= last_point_numbers[current_contour])
        {
            if(flags[i] & 1)
            {
                curve_points = create_array(2, 0);//create_array(2, destroy_point);

                point = create_point(2);

                point->coords[0] = coords[2*i];
                point->coords[1] = coords[2*i + 1];

                add_in_array(curve_points, point);


                point = create_point(2);

                point->coords[0] = ((Point*)first_contour_curve->points->data[0])->coords[0];
                point->coords[1] = ((Point*)first_contour_curve->points->data[0])->coords[1];

                add_in_array(curve_points, point);

                add_in_array(curves, create_curve(curve_points));
            }
            else
            {
                last_contour_curve = array_top(curves);

                curve_points = create_array(2, 0);//create_array(2, destroy_point);

                point = create_point(2);

                point->coords[0] = ((Point*)array_top( last_contour_curve->points ))->coords[0];
                point->coords[1] = ((Point*)array_top( last_contour_curve->points ))->coords[1];

                add_in_array(curve_points, point);


                point = create_point(2);

                point->coords[0] = coords[2*i];
                point->coords[1] = coords[2*i + 1];

                add_in_array(curve_points, point);


                point = create_point(2);

                point->coords[0] = ((Point*)first_contour_curve->points->data[0])->coords[0];
                point->coords[1] = ((Point*)first_contour_curve->points->data[0])->coords[1];

                add_in_array(curve_points, point);

                add_in_array(curves, create_curve(curve_points));
            }

            ++current_contour;
            first_contour_curve = 0;
            continue;
        }

        if(flags[i] & 1)
        {
            if((flags[i+1] & 1))
            {
                curve_points = create_array(2, 0);//create_array(2, destroy_point);


                point = create_point(2);

                point->coords[0] = coords[2*i];
                point->coords[1] = coords[2*i + 1];

                add_in_array(curve_points, point);


                point = create_point(2);

                point->coords[0] = coords[2*(i+1)];
                point->coords[1] = coords[2*(i+1) + 1];

                add_in_array(curve_points, point);

                add_in_array(curves, create_curve(curve_points));
            }
            else
                continue;
        }
        else
        {
            curve_points = create_array(3, 0);//create_array(3, destroy_point);

            point = create_point(2);

            if(flags[i-1] & 1)
            {
                point->coords[0] = coords[2*(i-1)];
                point->coords[1] = coords[2*(i-1) + 1];
            }
            else
            {
                point->coords[0] = (coords[2*(i-1)] + coords[2*i]) / 2.0f;
                point->coords[1] = (coords[2*(i-1) + 1] + coords[2*i + 1]) / 2.0f;
            }

            add_in_array(curve_points, point);

            point = create_point(2);

            point->coords[0] = coords[2*(i)];
            point->coords[1] = coords[2*(i) + 1];

            add_in_array(curve_points, point);

            point = create_point(2);

            if(flags[i+1] & 1)
            {
                point->coords[0] = coords[2*(i+1)];
                point->coords[1] = coords[2*(i+1) + 1];
            }
            else
            {
                point->coords[0] = (coords[2*(i+1)] + coords[2*i]) / 2.0f;
                point->coords[1] = (coords[2*(i+1) + 1] + coords[2*i + 1]) / 2.0f;
            }

            add_in_array(curve_points, point);

            add_in_array(curves, create_curve(curve_points));
        }

        if(!first_contour_curve)
        {
            first_contour_curve = array_top(curves);
        }
    }

    glyph->curves = curves;

    return glyph;
}


Array* load_TTF(Input *input)
{
    N_32  i;

    N_16  number_of_tables;
    N_16  search_range;
    N_16  entry_selector;
    N_16  range_shift;

    Byte  tag_name[5];
    N_32  checksum;
    N_32  offset;
    N_32  length;

    N_32  read_count;

    N_32  glyph_begin;
    N_32  glyph_length;


    Array *glyphs;

    glyphs = create_array(2, 0); //destroy_glyph

    tag_name[4] = '\0';

    if(!read_if_next_byte_array(input, "\x00\x01\x00\x00", 4))
    {
        goto error;
    }

    number_of_tables = read_binary_N_16(input);
    search_range     = read_binary_N_16(input);
    entry_selector   = read_binary_N_16(input);
    range_shift      = read_binary_N_16(input);

    for(i=0; i<number_of_tables; ++i)
    {
        read_byte_array(input, tag_name, 4);
        checksum = read_binary_N_32(input);
        offset   = read_binary_N_32(input);
        length   = read_binary_N_32(input);

        if( !strcmp(tag_name, "glyf") )
        {
            glyph_begin = offset;
        }
    }

    read_count = 20 + number_of_tables * 4;

    while(read_count != glyph_begin)
        read_byte(input);

    read_byte(input);

    for(i=0; i<95; ++i)
    {
        Vector_Glyph *glyph = read_TTF_glyph(input);

        if(!glyph)
            continue;

        add_in_array(glyphs, glyph);
    }

    return glyphs;

error:
    return 0;
}
*/


static void read_TTF_glyph(Input* input, Buffer* glyphs)
{
    Z_16 number_of_contours;
    N_16 min_x;
    N_16 min_y;
    N_16 max_x;
    N_16 max_y;

    number_of_contours = read_binary_Z_16(input);
    revert_bytes_order(&number_of_contours, 2);

    min_x = read_binary_N_16(input);
    min_y = read_binary_N_16(input);
    max_x = read_binary_N_16(input);
    max_y = read_binary_N_16(input);
    revert_bytes_order(&min_x, 2);
    revert_bytes_order(&min_y, 2);
    revert_bytes_order(&max_x, 2);
    revert_bytes_order(&max_y, 2);

    if(number_of_contours < 0)
    {
        log("complex glyph")
        return;
    }

    printf("(%d, %d, %d, %d)\n", min_x, min_y, max_x, max_y);
    printf("%d contours\n", number_of_contours);

    //number of points in current contour
    N_16 number_of_points;
    cycle(0, number_of_contours, 1)
        number_of_points = read_binary_N_16(input);
        revert_bytes_order(&number_of_points, 2);
    end
    ++number_of_points;
    printf("%d points\n", number_of_points);

    //skip instructions
    N_16 instructions_size = read_binary_N_16(input);
    revert_bytes_order(&instructions_size, 2);
    cycle(0, instructions_size, 1)
        read_binary_N_8(input);
    end

    BUFFER(options, number_of_points)
        cycle(0, number_of_points, 1)
            Byte option = read_binary_N_8(input);

            if(option & REPEAT_FLAG)
            {
                N_8 options_count = read_binary_N_8(input);
                add_in_buffer_end(&options, Byte, option);

                cycle(0, options_count, 1)
                    add_in_buffer_end(&options, Byte, option);
                end
                i += options_count;
            }
            else
            {
                add_in_buffer_end(&options, Byte, option);
            }
        end

        N_16* coords = malloc(sizeof(N_16) * number_of_points);

        Z_16 previouse_x = 0;
        cycle(0, number_of_points, 1)
            Byte option;
            get_buffer_bytes(&options, i, &option, 1);

            if(option & SHORT_X)
            {
                if(option & X_IS_POSITIVE)
                    previouse_x += read_binary_N_8(input);
                else
                    previouse_x -= read_binary_N_8(input);
            }
            else if(!(option & X_IS_SAME))
            {
                Z_16 delta_x = read_binary_Z_16(input);
                revert_bytes_order(&delta_x, 2);
                previouse_x += delta_x;
            }

            coords[i*2] = previouse_x;
        end

        Z_16 previouse_y = 0;
        cycle(0, number_of_points, 1)
            Byte option;
            get_buffer_bytes(&options, i, &option, 1);

            if(option & SHORT_Y)
            {
                if(option & Y_IS_POSITIVE)
                    previouse_y += read_binary_N_8(input);
                else
                    previouse_y -= read_binary_N_8(input);
            }
            else if(!(option & Y_IS_SAME))
            {
                Z_16 delta_y = read_binary_Z_16(input);
                revert_bytes_order(&delta_y, 2);
                previouse_y += delta_y;
            }

            coords[i*2 + 1] = previouse_y;
        end

        cycle(0, number_of_points, 1)
            printf("(%d, %d)\n", coords[i*2], coords[i*2 + 1]);
        end

        free(coords);
    END_BUFFER(options)
}


Boolean read_TTF(Buffer* glyphs, Byte* file_path)
{
    Boolean status = 1;

    N_16  number_of_tables;
    N_16  search_range;
    N_16  entry_selector;
    N_16  range_shift;

    Byte  tag_name[4];
    N_32  checksum;
    N_32  offset;
    N_32  length;

    initialize_buffer(glyphs, 100);
    FILE_INPUT(file_path)
        if(!read_if_bytes(&input, "\x00\x01\x00\x00", 4))
            goto error;

        number_of_tables = read_binary_N_16(&input);
        search_range     = read_binary_N_16(&input);
        entry_selector   = read_binary_N_16(&input);
        range_shift      = read_binary_N_16(&input);

        revert_bytes_order(&number_of_tables, 2);
        revert_bytes_order(&search_range, 2);
        revert_bytes_order(&entry_selector, 2);
        revert_bytes_order(&range_shift, 2);

        cycle(0, number_of_tables, 1)
            read_bytes(&input, tag_name, 4);

            checksum = read_binary_N_32(&input);
            offset   = read_binary_N_32(&input);
            length   = read_binary_N_32(&input);
            revert_bytes_order(&offset, 4);
            revert_bytes_order(&length, 4);

            if(
                tag_name[0] == 'g' &&
                tag_name[1] == 'l' &&
                tag_name[2] == 'y' &&
                tag_name[3] == 'f'
            )
            {
                set_file_position(&input, offset);
                read_TTF_glyph(&input, glyphs);

                read_binary_N_8(&input);
                read_TTF_glyph(&input, glyphs);

                goto success;
            }
        end

        goto error;

    error:
        log_error("ttf error")
        deinitialize_buffer(glyphs);
        status = 0;
    success:
    END_INPUT

    return status;
}