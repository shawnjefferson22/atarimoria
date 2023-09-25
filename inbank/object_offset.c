//int8 __fastcall__ object_offset(inven_type *t_ptr)
{
  switch (t_ptr->tval)
    {
    case TV_AMULET:             return(0);
    case TV_RING:               return(1);
    case TV_STAFF:              return(2);
    case TV_WAND:               return(3);
    case TV_SCROLL1:
    case TV_SCROLL2:            return(4);
    case TV_POTION1:
    case TV_POTION2:            return(5);
    case TV_FOOD:
      if ((t_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1)) < MAX_MUSH)
		return(6);
      return(-1);
    default:                    return(-1);
    }
}
