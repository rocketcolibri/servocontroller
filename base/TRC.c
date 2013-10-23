/**
 * Implementation of the trace module
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "GEN.h"
#include "MON.h"
#include "DBG.h"
#include "TRC.h"

#define GEN_MAX_NOF_THREADS 100
#define GEN_TRC_MAX_GROUPS 100

#define MAX_GRP_NAME_LEN 8
#define LINE_LEN  256
#define NOF_LINE  4

typedef struct
{
	UINT8 DispTrcLvl;
	char Name[MAX_GRP_NAME_LEN];
} TraceGroupEntry_t;

typedef struct
{
	BOOL TraceOutputOn;
	UINT8 NofGroups;
	UINT8 TaskIDs[GEN_MAX_NOF_THREADS];
	UINT8 TimeFormat;  // output format time:  0: --, 1: nn sec, 2: nn.mmm sec
	UINT8 NameFormat;  // output format name:  0: --, 1: filename.ext
	UINT8 LineFormat;  // output format line:  0: --, 1: nnn
	UINT8 GroupFormat; // output format group: 0: --, 1: group name
	UINT8 LevelFormat; // output format level: 0: --, 1: trace level
} TraceConfig_t;

static TraceConfig_t TraceConfig;
static TraceGroupEntry_t TraceGroup[GEN_TRC_MAX_GROUPS];

/**
 * @short TRC_AddTraceGroup
 *
 * Registers a new group and assigns a number which is returned
 *
 * @param group_name: ASCII-Z string
 * @return:  BOOL (always TRUE)
 */
UINT8 TRC_AddTraceGroup(char * group_name)
{
	DBG_ASSERT(TraceConfig.NofGroups < GEN_TRC_MAX_GROUPS);
	strncpy(TraceGroup[TraceConfig.NofGroups].Name, group_name,
			MAX_GRP_NAME_LEN - 1);
	return TraceConfig.NofGroups++;
}

/**
 * @short Prints a trace message depending on trace module settings.
 * Note:    Call this function by using macro TRC_PRINT.
 *
 * @param line:  line number (source file), inserted by macro
 * @param file:  file name (source file), inserted by macro
 * @param group: group number returned after calling TRC_AddTraceGroup()
 * @param lvl:   severity of trace message
 * @param fmt:   format string (as for printf)
 * @param ...:   variables as needed for fmt
 * @return:  FALSE if message was not sent to serial port due to lack of memory.
 */
BOOL TRC_Print(const char *file, const UINT16 line, const UINT8 group,
		const TRC_TraceLevel lvl, FILE *fout, const char *fmt, ...)
{
	va_list args;
	BOOL retval = TRUE;
	INT nchars = 0;
	INT wrpos = 0;

	if (TraceConfig.TraceOutputOn)
	{
		/* WARNING: THE FOLLOWING IS TEMPORARY FIX ONLY!!! */
		if (TraceConfig.TaskIDs[0] || TraceGroup[group].DispTrcLvl <= lvl)
		{
			char *pch;
			char *pstr = malloc(LINE_LEN);

			if (pstr == NULL )
				return FALSE;

			*pstr = '\0';           // initialize string
			pch = pstr;             // prepare pointer to append behind time
			switch (TraceConfig.TimeFormat)
			{
			case 1:
				sprintf(pstr, "\n\r%7d ", GEN_GetRTC());  // print nn sec
				pch = &pstr[10];
				break;
			case 2:
			{
				UINT32 time = GEN_GetRTC();                // print secound.millisec sec
				sprintf(pstr, "\n\r%7d.%03u ", time / 1000, (UINT16) (time % 1000));
			}
				pch = &pstr[14];
				break;
			}
			if (TraceConfig.NameFormat)
			{
				UINT8 filendx = strlen(file); // drop path in front of filename
				while (filendx && file[filendx - 1] != '\\')
					--filendx;
				sprintf(pch, "%10s ", &file[filendx]);
				pch = &pch[strlen(pch)];
			}
			if (TraceConfig.LineFormat)
			{
				sprintf(pch, "%-5u ", line);
				pch = &pch[strlen(pch)];
			}
			if (TraceConfig.GroupFormat)
			{
				sprintf(pch, "%-5s ", TraceGroup[group].Name);
				pch = &pch[strlen(pch)];
			}
			if (TraceConfig.LevelFormat)
			{
				sprintf(pch, "%u ", lvl);
				pch = &pch[strlen(pch)];
			}

			// --- get the start position
			wrpos = strlen(pstr);

			// --- Variable Argumentenliste an vsprintf weitergeben
			va_start(args, fmt);
			nchars = vsnprintf(&pstr[wrpos], LINE_LEN - wrpos, fmt, args);
			va_end(args);

			// --- set 0-termination character if string is too long
			if (nchars >= LINE_LEN - wrpos)
				pstr[LINE_LEN - 1] = '\0';

			if (fout == NULL )
			{
				// --- normal monitor output: MON_WriteInfo()
				//retval = MON_WriteInfo(pstr);
				fprintf(stderr, "%s", pstr);
			}
			else
			{
				// --- output in file
				fprintf(fout, "%s\n", pstr);
				fflush(fout);
			}

			free(pstr);
		}
	}
	return retval;
}

/**
 * @short Set the trace level of a group
 *
 * @param group: group number returned after calling TRC_AddTraceGroup()
 * @param lvl:   severity of trace message (set to TL_NOF_LVL to disable)
 * @return:  old trace level
 */
TRC_TraceLevel TRC_SetTraceLevel(const UINT8 group, const TRC_TraceLevel lvl)
{
	DBG_ASSERT(TraceConfig.NofGroups >= group);
	DBG_ASSERT(TL_NOF_LVL >= lvl);
	TRC_TraceLevel oldlvl = TraceGroup[group].DispTrcLvl;
	TraceGroup[group].DispTrcLvl = lvl;
	return oldlvl;
}

/**
 * @short Get the trace level of a group
 *
 * @param  group: group number returned after calling TRC_AddTraceGroup()
 * @return:  actual trace level
 */
TRC_TraceLevel TRC_GetTraceLevel(const UINT8 group)
{
	DBG_ASSERT(TraceConfig.NofGroups >= group);

	return TraceGroup[group].DispTrcLvl;
}


/******************************************************************************
 * TRC_ExecMonCmd
 ******************************************************************************
 * Purpose: Execute command received by the debug monitor. *cmdLine contains
 *          the parameters of the monitor command.
 *
 * Input:   UINT16 sNr (instance)
 *          char *cmdLine
 * Output:  various terminal messages
 * Return:  BOOL (always TRUE)
 * Note:    Call this function from the monitor task only.
 *****************************************************************************/
BOOL TRC_ExecMonCmd(UINT16 sNr, char *cmdLine)
{
	UINT8 argc;
	char** argv;

	MON_SplitArgs(cmdLine, &argc, &argv);
	MON_WriteInfof("\r\n");

	switch (argv[0][0])
	{
		case 'a':
		if(argc!=2)
		MON_WriteInfo("Use: trc a <task id>\r\n");
		else
		{
			UINT16 ii;
			ii = atoi(argv[1]);  // extract the task identifier
			if (ii >= GEN_MAX_NOF_THREADS)
			MON_WriteInfof("Only tasks 0..%u available\r\n", GEN_MAX_NOF_THREADS-1);
			else
			{
				TraceConfig.TaskIDs[ii] ^= 1;
				MON_WriteInfof(" Trace output for task id %u switched o%s\r\n",
						ii, TraceConfig.TaskIDs[ii] ? "n" : "ff");
			}
		}
		break;
#ifdef TEST
//#####################################################################################################
		case 'b':
		{
			static UINT8 current_group = 0;
			static UINT8 GrpNdx;
			static const char *GroupNames[] =
			{	"Mon", "B_Min", "C_Oli", "D_Maximilian", "E_Hop", "F_xxx"};
			current_group = TRC_AddTraceGroup(GroupNames[GrpNdx++]);
			MON_WriteInfof(" registered group: %d", current_group);
		}
		break;
		case 'c':
		{
			TRC_Print(__FILE__, __LINE__, /*group*/0, TL_ERROR, "| Text %u |", 192);
		}
		break;
//#####################################################################################################
#endif
		case 'd':
		MON_WriteInfof("Trace messages %sabled\r\n", TraceConfig.TraceOutputOn ? "en" : "dis");
		MON_WriteInfof("all levels enabled for task id:");
		for (argc = 0; argc < GEN_MAX_NOF_THREADS; ++argc)
		if (TraceConfig.TaskIDs[argc])
		MON_WriteInfof(" %d", argc);
		MON_WriteInfof("\r\n");
		if (TraceConfig.NofGroups)
		{
			MON_WriteInfof("gr name  lvl\r\n");
		}
		for (argc=0; argc<TraceConfig.NofGroups; ++argc)
		{
			MON_WriteInfof("%2d %-6s %u\r\n", argc, TraceGroup[argc].Name,
					TraceGroup[argc].DispTrcLvl);
		}
		break;

		case 'l':
		if(argc!=3)
		MON_WriteInfo("Use: trc l <group>,<level>\r\n");
		else
		{
			UINT16 gg, ll;
			gg = atoi(argv[1]);  // extract the trace group
			if (gg >= TraceConfig.NofGroups)
			MON_WriteInfof("Only groups 0..%u available\r\n",
					TraceConfig.NofGroups ? TraceConfig.NofGroups-1 : 0);
			else
			{
				ll = atoi(argv[2]);  // extract the new trace level
				if (ll > TL_NOF_LVL)
				MON_WriteInfof("Only levels 0..%u available\r\n", TL_NOF_LVL);
				else
				{
					TraceGroup[gg].DispTrcLvl = ll;
					MON_WriteInfof(" Trace level of group %u set to %u\r\n",
							gg, TraceGroup[gg].DispTrcLvl);
				}
			}
		}
		break;

		case 'o':
		if(argc!=3)
		MON_WriteInfo("Use: trc o <parameter>,<format>\r\n");
		else
		{
			const char ONLY_FMT_0_x_AVLBL[] = "Only format 0..%u available\r\n";
			const char OUT_FMT_x_SET_TO_n[] = " Output format for %s set to %s\r\n";
			UINT16 fm;
			fm = atoi(argv[2]);  // extract the new format
			switch (argv[1][0])// check the parameter
			{
				case 't':
				if (fm > 2)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 2);
				else
				{
					TraceConfig.TimeFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "time",
							fm == 0 ? "--" : fm == 1 ? "n sec" : fm == 2 ? " n.mmm sec" : "??");
				}
				break;
				case 'n':
				if (fm > 1)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 1);
				else
				{
					TraceConfig.NameFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "name",
							fm == 0 ? "--" : fm == 1 ? "name.ext" : "??");
				}
				break;
				case 'l':
				if (fm > 1)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 1);
				else
				{
					TraceConfig.LineFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "line",
							fm == 0 ? "--" : fm == 1 ? "'line'" : "??");
				}
				break;
				case 'g':
				if (fm > 1)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 1);
				else
				{
					TraceConfig.GroupFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "group",
							fm == 0 ? "--" : fm == 1 ? "'group'" : "??");
				}
				break;
				case 'v':
				if (fm > 1)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 1);
				else
				{
					TraceConfig.LevelFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "level",
							fm == 0 ? "--" : fm == 1 ? "'level'" : "??");
				}
				break;
				case 'a':
				if (fm > 1)
				MON_WriteInfof(ONLY_FMT_0_x_AVLBL, 1);
				else
				{
					TraceConfig.TimeFormat = TraceConfig.NameFormat =
					TraceConfig.LineFormat = TraceConfig.GroupFormat = TraceConfig.LevelFormat = fm;
					MON_WriteInfof(OUT_FMT_x_SET_TO_n, "all parameters",
							fm == 0 ? "--" : fm == 1 ? "default" : "??");
				}
				break;
				default:
				MON_WriteInfo("Only paramters N,T available\r\n");
			}
		}
		break;

		case 't':
		TraceConfig.TraceOutputOn ^= 1;
		MON_WriteInfof(" Trace output turned o%s\r\n",
				TraceConfig.TraceOutputOn ? "n" : "ff");
		break;

		case '?':
		default:
		MON_WriteInfo ("Usage:\r\n");
		MON_WriteInfof(" trc a i   Toggle all trace outputs for task id i on/off\r\n");
		MON_WriteInfof(" trc d     Display filter settings\r\n");
		MON_WriteInfof(" trc l g n Set new trace level n for group g\r\n");
		MON_WriteInfof(" trc o t n Set time format n  (0:no, 1:s 2:ms)\r\n");
		MON_WriteInfof("     o n n Set name format n  (0:no, 1:name.ext)\r\n");
		MON_WriteInfof("     o l n Set line format n  (0:no, 1:line number)\r\n");
		MON_WriteInfof("     o g n Set group format n (0:no, 1:group name)\r\n");
		MON_WriteInfof("     o v n Set level format n (0:no, 1:trace level)\r\n");
		MON_WriteInfof("     o a n Set all formats n  (0:no, 1:default)\r\n");
		MON_WriteInfof(" trc t     Toggle trace output on/off\r\n");
		break;
	}
	return(TRUE);
}
// end - SDSM_ExecMonCmd()


/**
 * TRC init function
 */
void TRC_Init()
{
	UINT8 ii;
	for (ii = 0; ii < GEN_TRC_MAX_GROUPS; ++ii)
		TraceGroup[ii].DispTrcLvl = TL_NOF_LVL;
	TraceConfig.TimeFormat = 1;
	TraceConfig.NameFormat = 1;
	TraceConfig.LineFormat = 1;
	TraceConfig.GroupFormat = 1;
	TraceConfig.LevelFormat = 1;

}
