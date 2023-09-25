/* Adjustment for charisma				-RAK-	  */
/* Percent decrease or increase in price of goods */
//int8u __fastcall__ chr_adj(void)
{
  register int8u charisma;


  charisma = py.stats.use_stat[A_CHR];
  if (charisma > 117)
    return(90);
  else if (charisma > 107)
    return(92);
  else if (charisma > 87)
    return(94);
  else if (charisma > 67)
    return(96);
  else if (charisma > 18)
    return(98);
  else
    switch(charisma) {
      case 18:	return(100);
      case 17:	return(101);
      case 16:	return(102);
      case 15:	return(103);
      case 14:	return(104);
      case 13:	return(106);
      case 12:	return(108);
      case 11:	return(110);
      case 10:	return(112);
      case 9:  return(114);
      case 8:  return(116);
      case 7:  return(118);
      case 6:  return(120);
      case 5:  return(122);
      case 4:  return(125);
      case 3:  return(130);
      default: return(100);
    }
}
