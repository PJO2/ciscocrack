//////////////////////////////////////////////////////
//
// Projet CISCOCRACK.   Mai 05 Ph.jounin
// File   ciscomain.c
//
//
//////////////////////////////////////////////////////


#define	 APP_TITLE				"CiscoCrack by Ph. Jounin"

// Icons
#define IDI_CISCOCRACK     11

// Dialog boxes
#define IDD_CISCOCRACK    101

// Items
#define IDC_CLEAR_PWD    1001
#define IDC_CRYPTED_PWD  1002

int cdecrypt(const char *enc_pw, char *dec_pw);

#define TASKTRAY_ID (* (DWORD *) "pJO8")
