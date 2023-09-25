#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_invcopy


/* Pushs a record back onto free space list             -RAK-   */
/* Delete_object() should always be called instead, unless the object in
   question is not in the dungeon, e.g. in store1.c and files.c */
//void __fastcall__ pusht(int8u x)
{
  register int8u i, j;
  register cave_type *c_ptr;


  if (x != tcptr - 1) {
    vbxe_bank(VBXE_OBJBANK);
    t_list[x] = t_list[tcptr - 1];

    /* must change the tptr in the cave of the object just moved */
    /*vbxe_bank(VBXE_CAVBANK);
    for (i = 0; i < cur_height; ++i)
     for (j = 0; j < cur_width; ++j) {
 	  c_ptr = CAVE_ADR(i, j);
 	    if (c_ptr->tptr == tcptr - 1)
 		  c_ptr->tptr = x;
       }*/

    /* must change the tptr in the cave of the object just moved */
    vbxe_bank(VBXE_CAVBANK);
    for (i = 0; i < cur_height; ++i) {
 	  c_ptr = CAVE_ADR(i, 0);
 	  for (j = 0; j < cur_width; ++j) {
 	    if (c_ptr->tptr == (tcptr - 1))
 	      c_ptr->tptr = x;
 	    ++c_ptr;
      }
    }
  }

  //vbxe_bank(VBXE_OBJBANK);
  --tcptr;
  INBANK_FUNCHELPER(&t_list[tcptr], OBJ_NOTHING);
}
