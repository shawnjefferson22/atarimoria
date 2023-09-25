//int8u __fastcall__ is_a_vowel(char ch)
{
  switch(ch) {
    case 'a': case 'e': case 'i': case 'o': case 'u':
    case 'A': case 'E': case 'I': case 'O': case 'U':
      return(TRUE);
    default:
      return(FALSE);
  }
}
