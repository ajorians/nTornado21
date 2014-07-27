#include <os.h>
#include "HighScoreEntry.h"
#include "Defines.h"

struct HighScoreEntry* CreateHighScoreEntry(struct HighScoreEntry** ppRoot, int nScore, const char* pstrName, int nNumber, int* pnIndex, const char* pstrExtra)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pCurrent = *ppRoot;
   struct HighScoreEntry* pPrevious = NULL;

   int nIndex = 0;
   while(pCurrent != NULL) {
      if( pCurrent->m_nScore <= nScore )//Keep it in order
         break;
      pPrevious = pCurrent;
      pCurrent = pCurrent->m_pNext;
      nIndex++;
   }

   struct HighScoreEntry* pEntry = malloc(sizeof(struct HighScoreEntry));
   if( pEntry == NULL ) {
      return NULL;
   }

   pEntry->m_nScore = nScore;
   strcpy(pEntry->m_strName, pstrName);
   pEntry->m_nNumber = nNumber;
   strcpy(pEntry->m_strExtraData, pstrExtra);

   //Add node into linked list
   pEntry->m_pNext = pCurrent;
   if( *ppRoot == NULL || nIndex == 0 ) {
      *ppRoot = pEntry;
   }
   else
   {
      pPrevious->m_pNext = pEntry;
   }
   if( pnIndex != NULL )
      *pnIndex = nIndex;

   return pEntry;
}

int ReadHighScoreEntries(struct HighScoreEntry** ppRoot, const char* pstr, const char* pstrEnd)
{
   DEBUG_FUNC_NAME;

   int nRet = HIGHSCORE_OK;
   int nStartSearch = 0;
   char* pstrFoundStart;
   char* pstrFoundScore;
   
   char* pstrFoundName;
   char* pstrFoundNameEnd;

   char* pstrFoundNumber;
   char* pstrFoundNumberEnd;

   char* pstrFoundExtraData;
   char* pstrFoundExtraDataEnd;

   char* pstrFoundEnd;
   while(1) {
      pstrFoundStart = strstr(pstr+nStartSearch, "<Entry score=\"");
      if( pstrFoundStart == NULL || pstrFoundStart > pstrEnd )
         break;
      pstrFoundScore = strstr(pstrFoundStart+strlen("<Entry score=\""), "\"");

      pstrFoundName = strstr(pstr+nStartSearch, "name=\"");
      if( pstrFoundName == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }
      pstrFoundNameEnd = strstr(pstrFoundName+strlen("name=\""), "\"");

      pstrFoundNumber = strstr(pstr+nStartSearch, "number=\"");
      if( pstrFoundNumber == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }
      pstrFoundNumberEnd = strstr(pstrFoundNumber+strlen("number=\""), "\"");
      if( pstrFoundNumberEnd == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }

      pstrFoundExtraData = strstr(pstrFoundNumberEnd, "extradata=\"");
      if( pstrFoundExtraData == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }

      pstrFoundExtraDataEnd = strstr(pstrFoundExtraData+strlen("extradata=\""), "\"");
      if( pstrFoundExtraDataEnd == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
      }

      pstrFoundEnd = strstr(pstrFoundExtraDataEnd, "/>");
      if( pstrFoundEnd == NULL ) {
         nRet = HIGHSCORE_READ_ERROR;
         break;
      }

      nStartSearch = pstrFoundEnd - pstr;

      char strBuffer[16];
      int nScoreLength = pstrFoundScore-pstrFoundStart-strlen("<Entry score=\"");
      strncpy(strBuffer, pstrFoundStart+strlen("<Entry score=\""), nScoreLength);
      strBuffer[nScoreLength] = '\0';
      int nScore = atoi(strBuffer);

      int nNumberLength = pstrFoundNumberEnd-pstrFoundNumber-strlen("number=\"");
      strncpy(strBuffer, pstrFoundNumber+strlen("number=\""), nNumberLength);
      strBuffer[nNumberLength] = '\0';
      int nNumber = atoi(strBuffer);

      char strName[MAX_NAME_LENGTH];
      int nNameLength = pstrFoundNameEnd-pstrFoundName-strlen("name=\"");
      strncpy(strName, pstrFoundName+strlen("name=\""), nNameLength);
      strName[nNameLength] = '\0';

      char strExtraData[MAX_EXTRA_DATA_LENGTH];
      int nExtraDataLength = pstrFoundExtraDataEnd-pstrFoundExtraData-strlen("extradata=\"");
      strncpy(strExtraData, pstrFoundExtraData+strlen("extradata=\""), nExtraDataLength);
      strExtraData[nExtraDataLength] = '\0';

      struct HighScoreEntry* pEntry = CreateHighScoreEntry(ppRoot, nScore, strName, nNumber, NULL/*Index*/, strExtraData/*Extra Data*/);
      if( pEntry == NULL ) {
         break;
      }
   }

   return nRet;
}

int PersistHighScoreEntry(struct HighScoreEntry* pRoot, FILE* fp)
{
   DEBUG_FUNC_NAME;

   char buffer[16];
   while(pRoot != NULL) {
      HIGHSCORE_WRITE("<Entry score=\"", strlen("<Entry score=\""), fp);
      HS_itoa(pRoot->m_nScore, buffer, 16);
      HIGHSCORE_WRITE(buffer, strlen(buffer), fp);
      HIGHSCORE_WRITE("\" name=\"", strlen("\" name=\""), fp);
      if( strlen(pRoot->m_strName) > 0 ) {
         HIGHSCORE_WRITE(pRoot->m_strName, strlen(pRoot->m_strName), fp);
      }
      HIGHSCORE_WRITE("\" number=\"", strlen("\" number=\""), fp);
      HS_itoa(pRoot->m_nNumber, buffer, 16);
      HIGHSCORE_WRITE(buffer, strlen(buffer), fp);
      HIGHSCORE_WRITE("\" extradata=\"", strlen("\" extradata=\""), fp);
      if( strlen(pRoot->m_strExtraData) > 0 ) {
         HIGHSCORE_WRITE(pRoot->m_strExtraData, strlen(pRoot->m_strExtraData), fp);
      }
      HIGHSCORE_WRITE("\"/>\n", strlen("\"/>\n"), fp);

      pRoot = pRoot->m_pNext;
   }

   return HIGHSCORE_OK;
}

int SetScoreNumberLimit(struct HighScoreEntry** ppEntry, int nLimit)
{
   DEBUG_FUNC_NAME;

   while(GetNumberEntries(*ppEntry) > nLimit) {
      RemoveEntry(ppEntry, nLimit);
   }

   return HIGHSCORE_OK;
}

void FreeHighScoreEntries(struct HighScoreEntry* pRoot)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pNode = pRoot;
   while(pNode != NULL) {
       pRoot = pNode->m_pNext;
       free(pNode);
       pNode = pRoot;
   }
}

int GetNumberEntries(struct HighScoreEntry* pEntry)
{
   DEBUG_FUNC_NAME;

   int nCount = 0;

   while(pEntry != NULL) {
      nCount++;
      pEntry = pEntry->m_pNext;
   }

   return nCount;
}

int RemoveEntry(struct HighScoreEntry** ppRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   int nCount = 0;
   struct HighScoreEntry* pPrevious = NULL;
   struct HighScoreEntry* pCurrent = *ppRoot;
   while(pCurrent != NULL) {
      if( nCount == nIndex )//Found it
         break;
      nCount++;
      pPrevious = pCurrent;
      pCurrent = pCurrent->m_pNext;
   }

   if( nCount < nIndex ) {//Not nIndex nodes
      return HIGHSCORE_INVALID_INDEX;
   }

   if( pPrevious == NULL ) {
      *ppRoot = pCurrent->m_pNext;
   }
   else {
      pPrevious->m_pNext = pCurrent->m_pNext;
   }
   free(pCurrent);

   return HIGHSCORE_OK;
}

struct HighScoreEntry* GetHSEntry(struct HighScoreEntry* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   int nCount = 0;
   while(pRoot != NULL) {
      if( nCount == nIndex )
         return pRoot;
      nCount++;
      pRoot = pRoot->m_pNext;
   }

   return NULL;
}

int GetHS(struct HighScoreEntry* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pHSE = GetHSEntry(pRoot, nIndex);
   if( pHSE == NULL )
      return -1;

   return pHSE->m_nScore;
}

const char* GetHSName(struct HighScoreEntry* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pHSE = GetHSEntry(pRoot, nIndex);
   if( pHSE == NULL )
      return "";

   return pHSE->m_strName;
}

int GetHSNumber(struct HighScoreEntry* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pHSE = GetHSEntry(pRoot, nIndex);
   if( pHSE == NULL )
      return -1;

   return pHSE->m_nNumber;
}

const char* GetHSExtraData(struct HighScoreEntry* pRoot, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScoreEntry* pHSE = GetHSEntry(pRoot, nIndex);
   if( pHSE == NULL )
      return "";

   return pHSE->m_strExtraData;
}


