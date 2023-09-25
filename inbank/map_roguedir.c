/* map rogue_like direction commands into numbers */
//char __fastcall__ map_roguedir(char comval)
{
  switch(comval) {
    case 0x1E:
    case 'h':
      comval = '4';
      break;
    case 'y':
      comval = '7';
      break;
    case 0x1C:
    case 'k':
      comval = '8';
      break;
    case 'u':
      comval = '9';
      break;
    case 0x1F:
    case 'l':
      comval = '6';
      break;
    case 'n':
      comval = '3';
      break;
    case 0x1D:
    case 'j':
      comval = '2';
      break;
    case 'b':
      comval = '1';
      break;
    case '.':
      comval = '5';
      break;
  }
  return(comval);
}
