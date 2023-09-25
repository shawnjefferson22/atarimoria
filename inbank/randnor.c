/* Generates a random integer number of NORMAL distribution -RAK-*/
//int16u __fastcall__ randnor(int16u mean, int16u stand)
{
  register int16u tmp, low, iindex, high;
  register int16 offset;


  vbxe_bank(VBXE_MAGBANK);					// bank in random normal table

  tmp = randint(MAX_SHORT);

  /* off scale, assign random value between 4 and 5 times SD */
  if (tmp == MAX_SHORT) {
    offset = 4 * stand + randint(stand);

    /* one half are negative */
    if (randint(2) == 1)
	  if (offset < mean)
	    offset = -offset;
	  else
	    offset = -mean;

    return mean + offset;
  }

  /* binary search normal normal_table to get index that matches tmp */
  /* this takes up to 8 iterations */
  low = 0;
  iindex = NORMAL_TABLE_SIZE >> 1;
  high = NORMAL_TABLE_SIZE;
  while (TRUE) {
    if ((normal_table[iindex] == tmp) || (high == (low+1)))
	  break;
    if (normal_table[iindex] > tmp) {
	  high = iindex;
	  iindex = low + ((iindex - low) >> 1);
	}
    else {
	  low = iindex;
	  iindex = iindex + ((high - iindex) >> 1);
	}
  }

  /* might end up one below target, check that here */
  if (normal_table[iindex] < tmp)
    iindex = iindex + 1;

  /* normal_table is based on SD of 64, so adjust the index value here,
     round the half way case up */
  offset = ((stand * iindex) + (NORMAL_TABLE_SD >> 1)) / NORMAL_TABLE_SD;

  /* one half should be negative */
  if (randint(2) == 1)
    if (offset < mean)
      offset = -offset;
    else
      offset = -mean;

  vbxe_restore_bank();
  return mean + offset;
}
