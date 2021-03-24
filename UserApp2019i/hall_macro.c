#include "e3user.h"
#include "usercomm.h"

int user_macro_6(macro_t *macro)
{
#define MAX_NFRAMES_LIMIT 100
    int            ii, iddummy=0,nframes; /*part_id, npoints,*/
    int            idc1[MAX_NFRAMES_LIMIT],idc2[MAX_NFRAMES_LIMIT];
    int            idp1[MAX_NFRAMES_LIMIT],idp2[MAX_NFRAMES_LIMIT];
    char           name[20];
    char           partname[80];
    char           pos[20], ass_pos[20];
    char           material[20];
    char           profile1[80], profile2[80];
    double          height1,height2,width,distance;
    point_         p1, *p_test;/*,p2,p3*/
    xs_line_t      line;
/*    xs_part_t      part;*/
/*    joint_parameter_t param;*/
/*    screw_parameter_t screw, screw2;*/
/*    welding_parameter_t welding;*/
    joint_t           joint;
/*    db_joint_t        e3joint;*/
    point_t            reference_point;/*xdir,*/
    point_             up;
    dia_joint_attr_t attributes;
    int              vertical_bolt_number, welding2_type, welding2_ftype;
    char             vertical_bolt_distances[100];
    int screw_standard;


    printf("user_macro_6 starts:\n");
    xsPolygonOrientation(1);

    xs_get_attribute_int("screw_standard",macro->id, &screw_standard);
    printf("screw_standard = %d \n", screw_standard);
    xs_get_attribute_int("bolt_or_hole",macro->id, &screw_standard);
    printf("bolt or hole = %d \n", screw_standard);
    xs_get_attribute_int("nut2",macro->id, &screw_standard);
    printf("nut2 = %d \n", screw_standard);
    xs_get_attribute_int("nut1",macro->id, &screw_standard);
    printf("nut1 = %d \n", screw_standard);
    xs_get_attribute_int("wash3",macro->id, &screw_standard);
    printf("wash3 = %d \n", screw_standard);
    xs_get_attribute_int("wash2",macro->id, &screw_standard);
    printf("wash2 = %d \n", screw_standard);
    xs_get_attribute_int("wash1",macro->id, &screw_standard);
    printf("wash1 = %d \n", screw_standard);


    printf("user_macro_6 starts:\n");

/**** only for test
    if(!xs_get_licence(DEVELOPER_ID_TEKLA, 6, 128))
        return 0;
*****/
    strcpy(name,"nframes");
    xs_get_attribute_int(name,macro->id,&nframes);

    strcpy(name,"distance");
    xs_get_attribute_float(name,macro->id,&distance);

    strcpy(name,"profile1");
    xs_get_attribute_string(name,macro->id, profile1);
    strcpy(name,"profile2");
    xs_get_attribute_string(name,macro->id, profile2);
    strcpy(name,"height1");
    xs_get_attribute_float(name,macro->id,&height1);

    strcpy(name,"height2");
    xs_get_attribute_float(name,macro->id,&height2);

    strcpy(name,"width");
    xs_get_attribute_float(name,macro->id,&width);

    printf("nframes=%d, distance=%f,height1=%f, height2=%f, width=%f \n",
	   nframes,distance,height1,height2,width);

/*test030996*/
    xs_get_attribute_int("nb",macro->id,&vertical_bolt_number);
    xs_get_attribute_string("lbd",macro->id, vertical_bolt_distances);
    xs_get_attribute_int("w2_type",macro->id,&welding2_type);
    xs_get_attribute_int("w2_ftype",macro->id,&welding2_ftype);
    printf("nb= %d, lbd= %s, w2_type = %d, w2_ftype = %d \n",
	  vertical_bolt_number, vertical_bolt_distances, welding2_type, welding2_ftype);
/*test030996*/


    xs_get_macro_input_one_point(macro->id,1,&p1);

    for (ii = 0; ii < nframes && ii < MAX_NFRAMES_LIMIT; ii++)
    {
	xs_point(&line.p1, p1.x, p1.y + (ii*distance), p1.z);
	xs_point_add(&line.p2, line.p1, 0.0, 0.0, height1);

	strcpy(ass_pos,"A_MACRO\\1");
	strcpy(pos,"XS\\2");
	strcpy(partname,"XS2COLUMN");
	strcpy(material,"FE360B");
	xs_part_attributes(ass_pos,pos,partname,material,1,2);
	xs_position_attributes(0.0,0.0,MIDDLE, 0.0, TOP, 0.0, MIDDLE, 0.0);
	idc1[ii] = xs_beam (profile1,line);
	xs_point(&line.p1, p1.x + width, p1.y + (ii*distance), p1.z);
	xs_point(&line.p2, p1.x + width, p1.y + (ii*distance), p1.z + height1);
	idc2[ii] = xs_beam (profile1,line);

	xs_position_attributes(0.0,0.0,MIDDLE, 0.0, TOP, 0.0, BELOW, 0.0);
	xs_point(&line.p1, p1.x, p1.y + (ii*distance), p1.z + height1);
	xs_point(&line.p2, p1.x + width/2.0, p1.y + (ii*distance), p1.z + height2);
	idp1[ii] = xs_beam (profile2,line);
	
	/*hp141194 added for test **/
	xs_point(&p_test, p1.x + width/2.0, p1.y + (ii*distance), p1.z + height2  + 345.0);
	xs_create_model_point(p_test);
	/*hp141194 added for test **/
	
	xs_point(&line.p1, p1.x + width/2.0, p1.y + (ii*distance), p1.z + height2);
	xs_point(&line.p2, p1.x + width, p1.y + (ii*distance), p1.z + height1);
	idp2[ii] = xs_beam (profile2,line);
	
	/* make joints */

	/**klevy**/
	up.x = 0.0;   up.y = 0.0;   up.z = 1000.0;
	xs_construct_joint_and_attribute_structs(&joint, &attributes, 
						 idc1[ii], 1, &idp1[ii], "klevy", KOLLISION_PLANE, 0, &up);
	if (!xs_get_joint_attributes_by_attr_name(29, "standard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110;
	    /*attributes.tj1 = 8.0;
	    attributes.btab1 = 1;*/
	    attributes.nw = 2; attributes.nb = 3;
	    strcpy(attributes.lwd,"200.0");
	    strcpy(attributes.lbd,"2*100.0");
	}
	
	if( !xs_create_joint(&joint, &attributes, ENDPLATE))
	    printf("klevy: did not succeed \n");
	
	/**doppel**/
	xs_construct_joint_and_attribute_structs(&joint, &attributes, 
						 idc2[ii], 1, &idp2[ii], "doppel",
						 KOLLISION_PLANE, 0, &up);
	if (!xs_get_joint_attributes_by_attr_name(3, "standard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110;
	    attributes.nw = 1; attributes.nb = 3;
	    attributes.nw2 = 2; 
	    strcpy(attributes.lwd2,"60.0");
	    strcpy(attributes.lbd,"60.0 80.0");
	    attributes.diameter = 12.0;
	    attributes.lwa = 60.0;
	}
	if( !xs_create_joint(&joint, &attributes, ANGLE))
	    printf("doppel: did not succeed \n");
	
	/**kstoss**/
	xs_construct_joint_and_attribute_structs(&joint, &attributes, 
						 idp1[ii], 1, &idp2[ii], "",
						 END_PLANE, 0, &up);
	if (!xs_get_joint_attributes_by_attr_name(14, "hallstandard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110; 
	    attributes.nw = 2; attributes.nb = 3;
	    strcpy(attributes.lwd,"200.0");
	    strcpy(attributes.lbd,"2*100.0");
	}
	if( !xs_create_joint(&joint, &attributes, END_PLATE_CONNECTION))
	    printf("kstoss: did not succeed \n");

	/**pohjalevy**/
	up.x = 1000.0;   up.y = 0.0;   up.z = 0.0;
	reference_point.x = 0.0; reference_point.y = 0.0; reference_point.z = 0.0;
	xs_construct_detail_and_attribute_structs(&joint, &attributes, 
						 idc1[ii], 0, &iddummy, "whatever",
						 KOLLISION_PLANE, 0, &up, &reference_point);
	if (!xs_get_joint_attributes_by_attr_name(1004, "hallstandard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110;
	    attributes.nw = 2; attributes.nb = 2;
	    strcpy(attributes.lwd,"500.0");
	    strcpy(attributes.lbd,"500.0");
	    attributes.tpl1 = 40.0;
	    attributes.diameter = 30.0;
	    attributes.rw1 = attributes.rw2 = 60.0;
	    attributes.rb1 = attributes.rb2 = 60.0;
	}
	if( !xs_create_joint(&joint, &attributes, BOTTOM_PLATE))
	    printf("pohjalevy: did not succeed \n");

	reference_point.x = width; reference_point.y = 0.0; reference_point.z = 0.0;
	xs_construct_detail_and_attribute_structs(&joint, &attributes, 
						 idc2[ii], 0, &iddummy, "whatever",
						 KOLLISION_PLANE, 0, &up, &reference_point);
	if (!xs_get_joint_attributes_by_attr_name(1004, "hallstandard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110;
	    attributes.nw = 2; attributes.nb = 2;
	    strcpy(attributes.lwd,"500.0");
	    strcpy(attributes.lbd,"500.0");
	    attributes.tpl1 = 40.0;
	    attributes.diameter = 30.0;
	    attributes.rw1 = attributes.rw2 = 60.0;
	    attributes.rb1 = attributes.rb2 = 60.0;
	}
	if( !xs_create_joint(&joint, &attributes, BOTTOM_PLATE))
	    printf("pohjalevy: did not succeed \n");
	
	up.x = -1000.0;   up.y = 0.0;   up.z = 0.0;
	reference_point.x = width; reference_point.y = ii*distance; reference_point.z = 1000.0;
	xs_construct_detail_and_attribute_structs(&joint, &attributes, 
						 idc2[ii], 0, &iddummy, "whatever",
						 KOLLISION_PLANE, 0, &up, &reference_point);

	if (!xs_get_joint_attributes_by_attr_name(1003, "hallstandard",
						  &attributes))
	{
	    attributes.screw_structure = 110; attributes.screw_structure2 = 110;
	    attributes.tpl1 = 10.0;
	    attributes.tpl2 = 8.0;
	}
	if( !xs_create_joint(&joint, &attributes, STIFFENER))
	    printf("jaykiste: did not succeed \n");
	
	printf("user_macro_6 ends:\n");
	
    } /*for-loop*/

    return 0;
}
