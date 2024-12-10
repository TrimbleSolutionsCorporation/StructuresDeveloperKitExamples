/*
 * round_plate_detail.c
 *
 * Detail: Round base plate

 * History:
 *		
 */

#include "e3user.h"

int user_detail_45(joint_t *joint, joint_parameter_t par, 
                   screw_parameter_t screw, welding_parameter_t welding)
{
        int                 i, result = 1, id_plate, id_pplate, nparts, partlist[MAXSCREW], ret;
	double              ang, off, hyp = 0.0, yy1, yy2, yy3, zz1, zz2, zz3, pi = 3.1415926535;
	point_             *spoints[6];
	point_t            *points[3];
	xs_line_t           line, line1;
        xs_polygon_t        polygon;
        profile_data_t      pdata;
        xs_screw_position_t screw_positions;

	xs_profile_data(joint->prim, &pdata);

	if (par.e[1] == DEF)
		par.e[1] = pdata.h/2;
	if (par.tpl[1] == DEF)
		par.tpl[1] = 20;
	if (par.hpl[1] == DEF)
		par.hpl[1] = 200;
	if (par.bpl[1] == DEF)
		par.bpl[1] = 100;
	if (par.bj[1] == DEF)
		par.bj[1] = 50;
	
	xs_point(&line.p1, par.tpl[1], -pdata.h/2, 0.0);
	xs_point_add(&line.p2, line.p1, 0.0, pdata.h, 0.0);
	
	result = xs_fitting(joint->prim, line);
	
	xs_part_attributes("RP\\1", joint->pos2, par.partname2, par.mat2,
		joint->group, joint->group2);
	
	xs_position_attributes(0, 0, LEFT, 0.0, TOP, 0.0, TOP, 0);
	
	xs_point(&points[0], 0, 0, 0);
	xs_point(&points[1], 0, pdata.h/2 + par.e[1], 0);
	xs_point(&points[2], 0, 0, 1);
	
	id_plate = xs_round_plate(par.tpl[1], points);
	
	if (id_plate)
		xs_create_welding_by_settings(&welding, 0, par.tpl[1], pdata.t, 
		par.tpl[1], pdata.t, WELD_DIR_PLUS_Y, joint->prim, id_plate);
	
	if (screw.nb == DEF)
		screw.nb = 4;
	if (screw.diameter == DEF)
		screw.diameter = 24;
        if (screw.tolerance == DEF)
		screw.tolerance  = 12.0;

        if (screw.lb[1] == DEF) screw.lb[1] = pdata.h*1.5;

	if (screw.nb > 0) 
	{
		ang = 360/screw.nb;
		off = ang/2;
		screw_positions.n = screw.nb;
		
		xs_get_current_plane_points(points);
		
		for (i = 0; i < screw.nb; i++)
		{
			hyp = screw.lb[1]/2; 
			screw_positions.x[i] = sin(i*ang*pi/180) * hyp;
			screw_positions.y[i] = cos(i*ang*pi/180) * hyp;
			
			hyp = pdata.h/2 - 2;
			yy1 = sin((i*ang + off)*pi/180) * hyp;
			yy2 = sin((i*ang + off)*pi/180) * (hyp + par.bj[1]);
			yy3 = sin((i*ang + off)*pi/180) * (hyp + par.bpl[1]);
			zz1 = cos((i*ang + off)*pi/180) * hyp;
			zz2 = cos((i*ang + off)*pi/180) * (hyp + par.bj[1]);
			zz3 = cos((i*ang + off)*pi/180) * (hyp + par.bpl[1]);
			
			hyp = pdata.h/2;
			yy1 = sin((i*ang + off)*pi/180) * hyp;
			zz1 = cos((i*ang + off)*pi/180) * hyp;
			
			xs_point(&line1.p1, par.bj[1] + par.tpl[1], yy1, zz1);
			xs_point(&line1.p2, par.hpl[1] + par.tpl[1], yy1, zz1);
			
			xs_point(&spoints[0], 0.0 + par.tpl[1], yy2, zz2);
			xs_point(&spoints[1], 0.0 + par.tpl[1], yy3, zz3);
			xs_point(&spoints[2], par.hpl[1]-par.bj[1] + par.tpl[1], yy3, zz3);
			xs_point(&spoints[3], par.hpl[1] + par.tpl[1], yy2, zz2);
			xs_point(&spoints[4], par.hpl[1] + par.tpl[1], yy1, zz1);
			xs_point(&spoints[5], par.bj[1] + par.tpl[1], yy1, zz1);

                        xs_part_attributes("ST\\1", joint->pos2, par.partname2, par.mat2,
				joint->group, joint->group2);
			xs_position_attributes(0, 0, MIDDLE, 0.0, TOP, 0.0, MIDDLE, 0);
			
			xs_set_plane(spoints[0], spoints[1], spoints[3]);
			
			xs_polygon(6, &spoints[0], &polygon);
			
			id_pplate = 0;
			if (par.atab[2]!=0) id_pplate = xs_polygon_plate(par.tpl[1], &polygon);
			
			if (id_pplate)
			{

				xs_fitting(joint->prim, line1);
				
				xs_create_welding_by_settings(&welding, 1, par.tpl[1], pdata.t, 
					par.tpl[1], pdata.t, WELD_DIR_MINUS_Y, id_plate, id_pplate);
				
				xs_create_welding_by_settings(&welding, 1, par.tpl[1], pdata.t,
					par.tpl[1], pdata.t, WELD_DIR_MINUS_X, joint->prim, id_pplate);
			}
			
			xs_set_plane(points[0], points[1], points[2]);
		}
		
		nparts = 1;
		partlist[0] = id_plate;
		
		xs_screw_usual_default_values(KSTIRN_SCREWS, LW_LB, &screw);
		
		xs_screw_attributes(screw.name, screw.diameter,
			par.tpl[1] + MAX(100, par.tpl[1]), joint->screwdin, screw.tolerance,
			screw.assembly_type, screw.long_hole_x, screw.long_hole_y,
			joint->group, screw.structure, screw.extralen, screw.thread_in);
		
		xs_position_attributes(0.0, 0.0, MIDDLE, 0.0, BOTTOM, 0.0, MIDDLE, 0.0);
		
		xs_point(&line.p1, par.tpl[1], 0.0, 0.0);
		xs_point(&line.p2, par.tpl[1], hyp, 0.0);

		ret = result;
		if (par.atab[1]!=0) ret = xs_screws(line, nparts, partlist, screw_positions);

                result = ret;
	}	
	return result;
}
