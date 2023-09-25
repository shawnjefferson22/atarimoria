#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  	name
#define INBANK_FUNCHELPER  	INBANK_NAME(INBANK_MOD)##_lite_spot


/* Change a trap from invisible to visible		-RAK-	*/
/* Note: Secret doors are handled here				 */
//void __fastcall__ change_trap(int8u y, int8u x)
{
  register inven_type *t_ptr;
  register int8u t;


  vbxe_bank(VBXE_CAVBANK);
  t = cave[y][x].tptr;						// make a local copy of tptr

  vbxe_bank(VBXE_OBJBANK);
  t_ptr = &t_list[t];
  if (t_ptr->tval == TV_INVIS_TRAP) {
    t_ptr->tval = TV_VIS_TRAP;
    INBANK_FUNCHELPER(y, x);
  }
  else if (t_ptr->tval == TV_SECRET_DOOR) {
    /* change secret door to closed door */
    t_ptr->index = OBJ_CLOSED_DOOR;
    t_ptr->tval = object_list[OBJ_CLOSED_DOOR].tval;
    t_ptr->tchar = object_list[OBJ_CLOSED_DOOR].tchar;
    INBANK_FUNCHELPER(y, x);
  }
}
