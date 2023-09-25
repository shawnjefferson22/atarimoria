#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_damroll

//int16u __fastcall__ pdamroll(int8u *array)
{
  return INBANK_FUNCHELPER(array[0], array[1]);
}