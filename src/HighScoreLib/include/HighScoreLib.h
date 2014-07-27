#ifndef HIGHSCORELIB_H_INCLUDED
#define HIGHSCORELIB_H_INCLUDED

typedef void* HighScoreLib;
typedef const char* Identifier;

#define HIGHSCORE_OK			(0)
#define HIGHSCORE_BADARGUMENT		(-1)
#define HIGHSCORE_UNABLE_OPEN_FILE	(-2)
#define HIGHSCORE_OUT_OF_MEMORY		(-3)
#define HIGHSCORE_UNABLE_DELETE_FILE	(-4)
#define HIGHSCORE_TABLE_NOT_CREATED	(-5)
#define HIGHSCORE_INVALID_INDEX		(-6)
#define HIGHSCORE_READ_ERROR		(-7)
#define HIGHSCORE_WRITE_ERROR		(-8)

#define HIGHSCORE_NOT_HIGH_ENOUGH	(1)
#define HIGHSCORE_BATCHING_OK		(2)

#define HIGHSCORE_NO_LIMIT		(0)

#define HIGHSCORE_ENABLE_BATCH		(1)
#define HIGHSCORE_DISABLE_BATCH		(0)

#define MAX_IDENTIFIER_LENGTH		(32)//If not big enough; let me know; in meantime adjust and recompile!
#define MAX_NAME_LENGTH			(64)
#define MAX_EXTRA_DATA_LENGTH		(64)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int HighScoreCreate(HighScoreLib* api);
int HighScoreFree(HighScoreLib* api);

int OpenHighScoreFile(HighScoreLib api, const char* pstrFile);
int GetHSError(HighScoreLib api);
void ClearHSError(HighScoreLib api);

int HSSetCurrentDirectory(const char* pstr);

int HSSetBatchMode(HighScoreLib api, int nState);

int HSCopyFile(const char* pstrCopyFrom, const char* pstrCopyTo);
int HSDeleteFile(const char* pstrFile);

//////////////////////////////////////////////
//Category related functions
//////////////////////////////////////////////
const char* GetCategoryID(HighScoreLib api);
int SetCategoryID(HighScoreLib api, Identifier ID);
int ClearCurrentCategoryID(HighScoreLib api);

//////////////////////////////////////////////
//High Score related functions
//////////////////////////////////////////////
int GetNumberScores(HighScoreLib api, Identifier ID);
int GetLimitNumber(HighScoreLib api);
int SetLimitNumber(HighScoreLib api, int nAmount);

int IsHighScore(HighScoreLib api, Identifier ID, int nScore);

int AddHighScore(HighScoreLib api, Identifier ID, int nScore, const char* pstrName, int nNumber, int* nIndex, const char* pstrExtra);

int ClearAllItems(HighScoreLib api, Identifier ID);

int RemoveItem(HighScoreLib api, Identifier ID, int nIndex);

int GetScore(HighScoreLib api, Identifier ID, int nIndex);
const char* GetName(HighScoreLib api, Identifier ID, int nIndex);
int GetNumber(HighScoreLib api, Identifier ID, int nIndex);
const char* GetExtraData(HighScoreLib api, Identifier ID, int nIndex);

#endif //HIGHSCORELIB_H_INCLUDED
