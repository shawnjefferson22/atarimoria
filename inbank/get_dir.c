#undef INBANK_NAME
#undef INBANK_FUNCHELPER

#define INBANK_NAME(name)  name
#define INBANK_FUNCHELPER  INBANK_NAME(INBANK_MOD)##_map_roguedir


/* Prompts for a direction				-RAK-	*/
/* Direction memory added, for repeated commands.  -CJS */
//int8u __fastcall__ get_dir(char *prompt, int8u *dir)
{
  char command;
  int8u save;
  //static char prev_dir;		/* Direction memory. -CJS- */		// moved to BSS -SJ
  extern int8u prev_dir;					/* Direction memory. -CJS- */


  if (default_dir)	/* used in counted commands. -CJS- */
  {
    *dir = prev_dir;
    return TRUE;
  }

  if (prompt == CNIL)
    prompt = "Which direction?";

  for (;;) {
    save = command_count;	/* Don't end a counted command. -CJS- */
	bank_prt(prompt, 0, 0, INBANK_RETBANK);

	/* Joystick movement -SJ */
	while (!kbhit()) {								// While no key pressed
	  if (STICK0 != 15) {							// if joystick moved
        command = joystick_command[STICK0];			// get the direction as command character
		break;
      }
	}

    if (kbhit()) {
 	  command = cgetc();
	  if (command == ESCAPE) {
	    free_turn_flag = TRUE;
	    return FALSE;
   	  }
	}

	bank_vbxe_cleartoeol(0, MSG_LINE, INBANK_RETBANK);

    command_count = save;
	//command = cast_map_roguedir(command);
	command = INBANK_FUNCHELPER(command);

    if (command >= '1' && command <= '9' && command != '5') {
	  prev_dir = command - '0';
	  *dir = prev_dir;
	  return TRUE;
	}
  }
}
