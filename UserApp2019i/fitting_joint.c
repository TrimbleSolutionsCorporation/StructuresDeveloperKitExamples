#include "e3user.h"

/* dummies */
int user_jointn_13(joint_t *joint, joint_parameter_t par, screw_parameter_t screw[],
                   welding_parameter_t welding)
{
    return 0;
}

int user_joint2_13(joint_t *joint, joint_parameter_t par, screw_parameter_t screw1,
                   screw_parameter_t screw2,welding_parameter_t welding)
{
    return 0;
}
/* end dummies */

int user_joint_13(joint_t *joint, joint_parameter_t par, screw_parameter_t screw,
	     welding_parameter_t welding)
{
#define SKEW_LIMIT_H 0.00001 /*0.000573 degrees*/
/******************************************************************/       
/* ANPASSEN_1                                                      */
/******************************************************************/     

/* internal variables */            
    int result = 1, result_weld = 1;
    xs_line_t line, gap_fitting_line;
    profile_data_t pdata, sdata;
    joint_parameter_t sub_par;        /* for horizontal stiffener */
    joint_t sub_joint;                /* for horizontal stiffener */
    welding_parameter_t tmp_welding;  /* for horizontal stiffener */
    double dy=0.0, dx=0.0;             /* for horizontal stiffener */
    joint_identifier_t joint_identifier;
    char pos1[80],pos2[80],pos3[80],pos4[80],mat[80];
    xs_section_t section;
    point_ *saveplane[3]; 
    xs_extrema_t sextr;
    double sec_h, gap_x, alfah, tanalfah;
/******************************************************************/
    xs_get_joint_default_pos_numbers_and_material(pos1, pos2, pos3, pos4, mat);

    if(par.weld   == DEF) par.weld = 0;
    if(par.dvl[1] == DEF) par.dvl[1] = 0;

    xs_point(&line.p1, par.dvl[1], -1000.0, 0.0);
    xs_point_add(&line.p2, line.p1, 0.0, 1000.0, 0.0);

    if ( (joint->point[5]->x - joint->point[4]->x) > 0.0)
        tanalfah = (joint->point[5]->y - joint->point[4]->y) / (joint->point[5]->x - joint->point[4]->x);		
    else 
        tanalfah = 0.0;
    if (par.col != DEF && (fabs(tanalfah) > SKEW_LIMIT_H))
    {
        xs_partbox_plane_intersection(joint->sec[0], line, &section);
	xs_get_current_plane_points(saveplane);
	xs_set_plane(joint->point[4], joint->point[5], joint->point[6]); /* skew joint plane */
	xs_part_extrema (joint->sec[0], &sextr);
	alfah = atan(tanalfah);
	sec_h = fabs(sextr.ymax - sextr.ymin);
	gap_x = fabs(sin(alfah)) * sec_h;
	if (par.col >= gap_x)
	{
	    if (alfah > 0)
	    {
		xs_point_add(&gap_fitting_line.p1, section.line1.p2, 0.0, 0.0, 0.0);
		xs_point_add(&gap_fitting_line.p2, gap_fitting_line.p1, 0.0, -sec_h, 0.0);
	    }
	    else
	    {
		xs_point_add(&gap_fitting_line.p1, section.line1.p1, 0.0, 0.0, 0.0);
		xs_point_add(&gap_fitting_line.p2, gap_fitting_line.p1, 0.0, sec_h, 0.0);
	    }
	    xs_fitting( joint->sec[0], gap_fitting_line);
	    xs_set_plane(saveplane[0], saveplane[1], saveplane[2]);
	}
	else
	{
	    xs_set_plane(saveplane[0], saveplane[1], saveplane[2]);
	    xs_fitting( joint->sec[0], section.line1 );
	}
    }
    else
	xs_fitting( joint->sec[0], line);

/* welding */
    if (par.weld) 
    {
	xs_profile_data(joint->prim, &pdata);
	xs_profile_data(joint->sec[0], &sdata);
	if (welding.type[0] || welding.type2[0]) /*sec top */
	{
	    if (!xs_create_welding_by_settings(&welding, 0, pdata.t, sdata.t, pdata.t, sdata.t,
					       WELD_DIR_PLUS_Y, joint->prim, joint->sec[0]))
		result_weld = 0;
	}
	if (welding.type[1] || welding.type2[1]) /*sec bottom*/
	{
	    if (!xs_create_welding_by_settings(&welding, 1, pdata.t, sdata.t2, pdata.t, sdata.t2,
					       WELD_DIR_MINUS_Y, joint->prim, joint->sec[0]))
		result_weld = 0;
	}
	if (welding.type[2] || welding.type2[2]) /*sec web */
	{
	    if (!xs_create_welding_by_settings(&welding, 2, pdata.s, sdata.s, pdata.s, sdata.s,
					       WELD_DIR_PLUS_Z, joint->prim, joint->sec[0]))
		result_weld = 0;
	}
    }

/* Make cuts if necessary */
    xs_notch(joint->prim, joint->sec[0]);

/* horizontal stiffeners */
    if(par.tj[1]!=DEF || par.bj[1]!=DEF || par.hj[1]!=DEF || par.tj[2]!=DEF || par.bj[2]!=DEF || par.hj[2]!=DEF) /* make stiffeners */
    {
        sub_par = par;
        sub_joint = *joint;

        strcpy(sub_joint.pos1, joint->pos5);
        strcpy(sub_joint.pos5, joint->pos6);
        if(sub_joint.pos1[0] == 0)
            strcpy(sub_joint.pos1, pos2);
        if(sub_joint.pos5[0] == 0)
            strcpy(sub_joint.pos5, pos2);

	tmp_welding=welding;
	xs_cp_welding(3,0,&welding);

	xs_profile_data(joint->sec[0], &sdata);
	if(sub_par.hj[1]==DEF) sub_par.hj[1]=sdata.h;
	if(sub_par.hj[2]==DEF) sub_par.hj[2]=sdata.h;
	if(sub_par.adist[1]==DEF) sub_par.adist[1]=sdata.h/4;
	if(sub_par.adist[2]==DEF) sub_par.adist[2]=sdata.h/4;

	dx=joint->point[5]->x;
	dy=(joint->point[5]->y - joint->point[1]->y);

	xs_point(&sub_joint.reference_point, 
		 joint->point[0]->x+(sqrt(dx*dx + dy*dy)/dx)*(par.dvl[1]),
		 joint->point[0]->y,
		 joint->point[0]->z);

	xs_point(&sub_joint.xdir, joint->point[5]->x, joint->point[5]->y, joint->point[5]->z);
	xs_point(&sub_joint.up, joint->point[6]->x, joint->point[6]->y, joint->point[6]->z);

	sub_joint.prim=joint->sec[0];

	sub_par.btab[1] = 13; /* indicates joint number */

	xs_profile_data(joint->sec[0], &sdata);
	sub_par.bdist[3] = dy / dx; /* angle_correction */

        joint_identifier.developer_id = DID_XSTEEL;
        joint_identifier.joint_number = HORIZONTAL_STIFFENER;

	if(!xs_detail(&sub_joint, joint_identifier, sub_par, screw, welding)) result = 0;
	welding=tmp_welding;
    }

    return result;
}                                    
