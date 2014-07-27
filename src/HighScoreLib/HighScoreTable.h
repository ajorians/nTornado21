#ifndef HIGHSCORETABLE_H
#define HIGHSCORETABLE_H

#include "include/HighScoreLib.h"
#include "HighScoreEntry.h"

struct HighScoreTable
{
   char m_strIdentifierName[MAX_IDENTIFIER_LENGTH];
   struct HighScoreEntry* m_pEntries;
   struct HighScoreTable* m_pNext;
};

void FreeHighScoreTables(struct HighScoreTable* pRoot);

int ReadHighScoreTables(struct HighScoreTable** ppRoot, const char* pstr);
int PersistHighScoreTables(struct HighScoreTable* pRoot, FILE* fp);

int SetTableLimit(struct HighScoreTable* pRoot, int nLimit);

int GetNumberOfHighScoreTables(struct HighScoreTable* pRoot);

struct HighScoreTable* GetHighScoreTableFromIndex(struct HighScoreTable* pRoot, int nIndex);

struct HighScoreTable* GetHighScoreTable(struct HighScoreTable* pRoot, Identifier ID);

struct HighScoreTable* CreateHighScoreTable(struct HighScoreTable** ppRoot, Identifier ID);

int GetNumberOfScores(struct HighScoreTable* pTable);

int IsHighScoreForTable(struct HighScoreTable* pTable, int nScore, int nLimit);

int AddHighScoreEntry(struct HighScoreTable* pRoot, int nScore, const char* pstrName, int nNumber, int* pnIndex, const char* pstrExtra);

int ClearAllEntries(struct HighScoreTable* pRoot);

int RemoveScoreIndex(struct HighScoreTable* pRoot, int nIndex);

int GetHighScore(struct HighScoreTable* pRoot, int nIndex);
const char* GetHighScoreName(struct HighScoreTable* pRoot, int nIndex);
int GetHighScoreNumber(struct HighScoreTable* pRoot, int nIndex);
const char* GetHighScoreExtraData(struct HighScoreTable* pRoot, int nIndex);


#endif

