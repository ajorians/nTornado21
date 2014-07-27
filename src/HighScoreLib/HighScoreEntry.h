#ifndef HIGHSCOREENTRY_H
#define HIGHSCOREENTRY_H

#include "include/HighScoreLib.h"

struct HighScoreEntry
{
   int   m_nScore;
   char	 m_strName[MAX_NAME_LENGTH];
   int   m_nNumber;//Able to use it as seen fit; i.e. use for time
   char  m_strExtraData[MAX_EXTRA_DATA_LENGTH];
   struct HighScoreEntry* m_pNext;
};

struct HighScoreEntry* CreateHighScoreEntry(struct HighScoreEntry** ppRoot, int nScore, const char* pstrName, int nNumber, int* pnIndex, const char* pstrExtra);

int ReadHighScoreEntries(struct HighScoreEntry** ppRoot, const char* pstr, const char* pstrEnd);
int PersistHighScoreEntry(struct HighScoreEntry* pRoot, FILE* fp);

int SetScoreNumberLimit(struct HighScoreEntry** ppRoot, int nLimit);

void FreeHighScoreEntries(struct HighScoreEntry* pRoot);

int GetNumberEntries(struct HighScoreEntry* pEntry);

int RemoveEntry(struct HighScoreEntry** ppRoot, int nIndex);

int GetHS(struct HighScoreEntry* pRoot, int nIndex);
const char* GetHSName(struct HighScoreEntry* pRoot, int nIndex);
int GetHSNumber(struct HighScoreEntry* pRoot, int nIndex);
const char* GetHSExtraData(struct HighScoreEntry* pRoot, int nIndex);

#endif

