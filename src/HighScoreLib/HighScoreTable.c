#include <os.h>
#include "HighScoreTable.h"
#include "HighScoreEntry.h"
#include "Defines.h"

void FreeHighScoreTables(struct HighScoreTable* pRoot)
{
   DEBUG_FUNC_NAME;

   struct HighScoreTable* pNode = pRoot;
   while(pNode != NULL) {
       pRoot = pNode->m_pNext;
       FreeHighScoreEntries(pNode->m_pEntries);
       free(pNode);
       pNode = pRoot;
   }
}

int ReadHighScoreTables(struct HighScoreTable** ppRoot, const char* pstr)
{
   DEBUG_FUNC_NAME;

   int nRet = HIGHSCORE_OK;
   int nStartSearch = 0;
   char* pstrFoundStart;
   char* pstrFoundName;
   char* pstrFoundEnd;
   while(1) {
      pstrFoundStart = strstr(pstr+nStartSearch, "<Category name=\"");
      if( pstrFoundStart == NULL )
         break;
      pstrFoundName = strstr(pstrFoundStart+strlen("<Category name=\""), "\"");
      pstrFoundEnd = strstr(pstrFoundStart, "</Category>\n");
      nStartSearch = pstrFoundEnd - pstr;
      if( pstrFoundName == NULL || pstrFoundEnd == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }

      char strCategory[MAX_IDENTIFIER_LENGTH];
      int nNameLength = pstrFoundName-strlen("<Category name=\"")-pstrFoundStart;
      strncpy(strCategory, pstrFoundStart+strlen("<Category name=\""), nNameLength);
      strCategory[nNameLength] = '\0';
      struct HighScoreTable* pHST = CreateHighScoreTable(ppRoot, strCategory);

      nRet = ReadHighScoreEntries(&pHST->m_pEntries, pstrFoundName, pstrFoundEnd);
   }

   return nRet;
}

int PersistHighScoreTables(struct HighScoreTable* pRoot, FILE* fp)
{
   DEBUG_FUNC_NAME;

   int nRet = HIGHSCORE_OK;

   while( pRoot != NULL ) {
      HIGHSCORE_WRITE("<Category name=\"", strlen("<Category name=\""), fp);
      if( strlen(pRoot->m_strIdentifierName) > 0 ) {
         HIGHSCORE_WRITE(pRoot->m_strIdentifierName, strlen(pRoot->m_strIdentifierName), fp);
      }
      HIGHSCORE_WRITE("\">\n", strlen("\">\n"), fp);

      nRet = PersistHighScoreEntry(pRoot->m_pEntries, fp);
      if( nRet != HIGHSCORE_OK ) {
         return nRet;
      }

      HIGHSCORE_WRITE("</Category>\n", strlen("</Category>\n"), fp);

      pRoot = pRoot->m_pNext;
   }
   return nRet;
}

int SetTableLimit(struct HighScoreTable* pRoot, int nLimit)
{
   DEBUG_FUNC_NAME;

   while( pRoot != NULL ) {
      SetScoreNumberLimit(&pRoot->m_pEntries, nLimit);
      pRoot = pRoot->m_pNext;
   }
   return HIGHSCORE_OK;
}

int GetNumberOfHighScoreTables(struct HighScoreTable* pRoot)
{
   DEBUG_FUNC_NAME;

   int nCount = 0;
   while( pRoot != NULL ) {
      nCount++;
      pRoot = pRoot->m_pNext;
   }
   return nCount;
}

struct HighScoreTable* GetHighScoreTableFromIndex(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   int nCount = 0;
   while( pRoot != NULL ) {
      if( nCount == nIndex )
         return pRoot;
      nCount++;
      pRoot = pRoot->m_pNext;
   }
   return NULL;
}

struct HighScoreTable* GetHighScoreTable(struct HighScoreTable* pRoot, Identifier ID)
{
   DEBUG_FUNC_NAME;

   if( pRoot == NULL ) {//Not found
      return NULL;
   }

   if( !strcmp(pRoot->m_strIdentifierName, ID) )//Matches
      return pRoot;

   return GetHighScoreTable(pRoot->m_pNext, ID);
}

struct HighScoreTable* CreateHighScoreTable(struct HighScoreTable** ppRoot, Identifier ID)
{
   DEBUG_FUNC_NAME;

   struct HighScoreTable* pCurrent = *ppRoot;
   struct HighScoreTable* pPrevious = NULL;

   while(pCurrent != NULL) {
      pPrevious = pCurrent;
      pCurrent = pCurrent->m_pNext;
   }

   struct HighScoreTable* pTable = malloc(sizeof(struct HighScoreTable));
   if( pTable == NULL ) {
      return NULL;
   }

   strcpy(pTable->m_strIdentifierName, ID);
   pTable->m_pEntries = NULL;

   //Add node into linked list
   pTable->m_pNext = pCurrent;
   if( *ppRoot == NULL ) {
      *ppRoot = pTable;
   }
   else
   {
      pPrevious->m_pNext = pTable;
   }

   return pTable;
}

int GetNumberOfScores(struct HighScoreTable* pTable)
{
   DEBUG_FUNC_NAME;

   return GetNumberEntries(pTable->m_pEntries);
}

int IsHighScoreForTable(struct HighScoreTable* pTable, int nScore, int nLimit)
{
   DEBUG_FUNC_NAME;

   if( nLimit < 0 ) {//If no limit then it is always a high enough score.
      return 1;
   }

   int i;
   int nNumberOfScores = GetNumberOfScores(pTable);
   if( nNumberOfScores < nLimit ) { //There is space for another high score
      return 1;
   }

   int nCompareScores = nLimit < nNumberOfScores ? nLimit : nNumberOfScores;

   for(i=0; i<nCompareScores; i++) {
      int n = GetHighScore(pTable, i);
      if( nScore >= n ) {
         return 1;
      }
   }

   return 0;
}

int AddHighScoreEntry(struct HighScoreTable* pRoot, int nScore, const char* pstrName, int nNumber, int* pnIndex, const char* pstrExtra)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pEntry = CreateHighScoreEntry(&pRoot->m_pEntries, nScore, pstrName, nNumber, pnIndex, pstrExtra);

   if( pEntry == NULL ) {
      return HIGHSCORE_OUT_OF_MEMORY;
   }

   return HIGHSCORE_OK;
}

int ClearAllEntries(struct HighScoreTable* pRoot)
{
   DEBUG_FUNC_NAME;

   while(GetNumberOfScores(pRoot) > 0) {
      RemoveScoreIndex(pRoot, 0);
   }

   return HIGHSCORE_OK;
}

int RemoveScoreIndex(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   return RemoveEntry(&pRoot->m_pEntries, nIndex);
}

int GetHighScore(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   return GetHS(pRoot->m_pEntries, nIndex);
}

const char* GetHighScoreName(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   return GetHSName(pRoot->m_pEntries, nIndex);
}

int GetHighScoreNumber(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   return GetHSNumber(pRoot->m_pEntries, nIndex);
}

const char* GetHighScoreExtraData(struct HighScoreTable* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   return GetHSExtraData(pRoot->m_pEntries, nIndex);
}

