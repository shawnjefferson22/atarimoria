/* Return a string describing how a given equipment item is carried. -CJS- */
//char* __fastcall__ describe_use(int8u i)
{
  char *p;


  switch(i) {
    case INVEN_WIELD:
      p = "wielding"; break;
    case INVEN_HEAD:
      p = "wearing on your head"; break;
    case INVEN_NECK:
      p = "wearing around your neck"; break;
    case INVEN_BODY:
      p = "wearing on your body"; break;
    case INVEN_ARM:
      p = "wearing on your arm"; break;
    case INVEN_HANDS:
      p = "wearing on your hands"; break;
    case INVEN_RIGHT:
      p = "wearing on your right hand"; break;
    case INVEN_LEFT:
      p = "wearing on your left hand"; break;
    case INVEN_FEET:
      p = "wearing on your feet"; break;
    case INVEN_OUTER:
      p = "wearing about your body"; break;
    case INVEN_LIGHT:
      p = "using to light the way"; break;
    case INVEN_AUX:
      p = "holding ready by your side"; break;
    default:
      p = "carrying in your pack"; break;
  }

  return p;
}
