//Public domain :)

#include <os.h>
#include "include/HighScoreLib.h"
#include "HighScoreTable.h"
#include "Defines.h"

char g_strCurrentPath[MAX_PATH];

struct HighScore
{
   int m_nLastError;					//The last error
   int m_nLimitItems;					//The number of items to hold; -1 for no limit
   int m_nBatch;					//Whether setting many files at once
   char m_strHighScoreFile[MAX_PATH];			//The filename of the highscore file
   struct HighScoreTable* m_pRootHighScoreTables;	//The root High score table data

   char m_strCurrentCategory[MAX_IDENTIFIER_LENGTH];
};

int HighScoreCreate(HighScoreLib* api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = malloc(sizeof(struct HighScore));
   if( pHS == NULL ){//Out of memory
      return HIGHSCORE_OUT_OF_MEMORY;
   }

   pHS->m_nLastError = HIGHSCORE_OK;
   pHS->m_nLimitItems = DEFAULT_LIMIT_HIGHSCORE_ITEMS;
   pHS->m_nBatch = HIGHSCORE_DISABLE_BATCH;
   strcpy(pHS->m_strHighScoreFile, "");
   pHS->m_pRootHighScoreTables = NULL;
   strcpy(pHS->m_strCurrentCategory, "");

   *api = pHS;
   return HIGHSCORE_OK;
}

int HighScoreFree(HighScoreLib* api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = *api;

   struct HighScoreTable* pHST = pHS->m_pRootHighScoreTables;
   FreeHighScoreTables(pHST);

   free(pHS);
   *api = NULL;
   return HIGHSCORE_OK;
}

int OpenHighScoreFile(HighScoreLib api, const char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   if( pstrFile == NULL ) {
      pHS->m_nLastError = HIGHSCORE_BADARGUMENT;
      return HIGHSCORE_BADARGUMENT;
   }

   char strPath[MAX_PATH];

   //Does it start with a '/'; if not think of it as a relative path
   if( (*pstrFile) == '/' ) {
      strcpy(strPath, "");
   }
   else {
      strcpy(strPath, g_strCurrentPath);
   }
   strcat(strPath, pstrFile);

   if( strlen(pHS->m_strHighScoreFile) ) {//Called with a file already opened; in this case I just reset; though the behavior could be changed if desired
      struct HighScoreTable* pHST = pHS->m_pRootHighScoreTables;
      FreeHighScoreTables(pHST);

      pHS->m_nLastError = HIGHSCORE_OK;
      pHS->m_nLimitItems = DEFAULT_LIMIT_HIGHSCORE_ITEMS;
      pHS->m_nBatch = HIGHSCORE_DISABLE_BATCH;
      strcpy(pHS->m_strHighScoreFile, "");
      pHS->m_pRootHighScoreTables = NULL;
      strcpy(pHS->m_strCurrentCategory, "");
   }

   strcpy(pHS->m_strHighScoreFile, strPath);

   struct stat statbuf;
   if (stat(strPath, &statbuf) == -1) {
      pHS->m_nLastError = HIGHSCORE_UNABLE_OPEN_FILE;
      return HIGHSCORE_UNABLE_OPEN_FILE;//This isn't an error; I probably could have called this function something
      //better; open kinda implies an existing file.
   }

   if( statbuf.st_size <= 0 ) {
      pHS->m_nLastError = HIGHSCORE_UNABLE_OPEN_FILE;
      return HIGHSCORE_UNABLE_OPEN_FILE;//Again probably should call this SetHSFileName instead of Open; as this
      //isn't an error.
   }

   FILE *fp = fopen(strPath, "r");
   int nRet = HIGHSCORE_READ_ERROR;
   if (fp) {
      char* pstrData = malloc(statbuf.st_size);
      if( pstrData == NULL ) {//Out of memory
         pHS->m_nLastError = HIGHSCORE_OUT_OF_MEMORY;
         fclose(fp);
         return HIGHSCORE_OUT_OF_MEMORY;
      }

      int nRead = fread(pstrData, 1, statbuf.st_size, fp);
      if( pstrData != NULL ) {
         if( NULL != strstr(pstrData, "<HighScoreTable version=\"1.0\"") ) {
            char* pstrFoundLimit = strstr(pstrData, "Limit=\"");
            if( pstrFoundLimit != NULL ) {
               char* pstrFoundLimitEnd = strstr(pstrFoundLimit+strlen("Limit=\""), "\"");

               char strLimit[16];
               int nLimitLength = pstrFoundLimitEnd-strlen("Limit=\"")-pstrFoundLimit;
               strncpy(strLimit, pstrFoundLimit+strlen("Limit=\""), nLimitLength);
               strLimit[nLimitLength] = '\0';
               int nLimit = atoi(strLimit);
               pHS->m_nLimitItems = nLimit;
            }

            nRet = ReadHighScoreTables(&pHS->m_pRootHighScoreTables, pstrData);
         }
      }

      free(pstrData);
      fclose(fp);
   }
   return nRet;
}

int Persist(struct HighScore* pHS)
{
   DEBUG_FUNC_NAME;

   int nRet = HIGHSCORE_OK;

   FILE *fp = fopen(pHS->m_strHighScoreFile, "w");
   if (fp) {
      const char* pstrXMLHeader = "<?xml version=\"1.0\"?>\n";
      fwrite(pstrXMLHeader, strlen(pstrXMLHeader), 1, fp);

      const char* pstrHeader = "<HighScoreTable version=\"1.0\"";
      fwrite(pstrHeader, strlen(pstrHeader), 1, fp);

      const char* pstrLimit = " Limit=\"";
      fwrite(pstrLimit, strlen(pstrLimit), 1, fp);

      char buffer[16];
      HS_itoa(pHS->m_nLimitItems, buffer, 16); 
      fwrite(buffer, strlen(buffer), 1, fp);

      const char* pstrCloseTag = "\">\n";
      fwrite(pstrCloseTag, strlen(pstrCloseTag), 1, fp);

      nRet = PersistHighScoreTables(pHS->m_pRootHighScoreTables, fp);
      if( nRet != HIGHSCORE_OK ) {
         pHS->m_nLastError = nRet;
         fclose(fp);
         return nRet;
      }

      const char* pstrFooter = "</HighScoreTable>\n";
      fwrite(pstrFooter, strlen(pstrFooter), 1, fp);

      fclose(fp);
   }
   else {
      pHS->m_nLastError = HIGHSCORE_UNABLE_OPEN_FILE;
      return HIGHSCORE_UNABLE_OPEN_FILE;
   }

   return nRet;
}

int GetHSError(HighScoreLib api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   return pHS->m_nLastError;
}

void ClearHSError(HighScoreLib api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   pHS->m_nLastError = HIGHSCORE_OK;
}

int HSSetCurrentDirectory(const char* pstr)
{
   DEBUG_FUNC_NAME;

   int nLastSlash = strlen(pstr) - 1;
   while(nLastSlash >= 0 ){
      if( *(pstr + nLastSlash) == '/' )
         break;//This had better break somewhere!
      nLastSlash--;
   }
   int n;
   for(n=0; n<=nLastSlash; n++)
      g_strCurrentPath[n] = *(pstr + n);

   return HIGHSCORE_OK;
}

int HSSetBatchMode(HighScoreLib api, int nState)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   if( pHS->m_nBatch == (nState ? HIGHSCORE_ENABLE_BATCH : HIGHSCORE_DISABLE_BATCH) )//Same mode; no effect
      return HIGHSCORE_OK;

   pHS->m_nBatch = nState ? HIGHSCORE_ENABLE_BATCH : HIGHSCORE_DISABLE_BATCH;

   if( !pHS->m_nBatch ) {
      return Persist(pHS);//Not batching calls anymore; so persist any changes
   }

   return HIGHSCORE_OK;
}

int HSCopyFile(const char* pstrCopyFrom, const char* pstrCopyTo)
{
   DEBUG_FUNC_NAME;

   if( pstrCopyFrom == NULL || pstrCopyTo == NULL ) {
      return HIGHSCORE_BADARGUMENT;
   }

   char strPathFrom[MAX_PATH];
   char strPathTo[MAX_PATH];

   //Does it start with a '/'; if not think of it as a relative path
   if( (*pstrCopyFrom) == '/' ) {
      strcpy(strPathFrom, "");
   }
   else {
      strcpy(strPathFrom, g_strCurrentPath);
   }
   strcat(strPathFrom, pstrCopyFrom);

   if( (*pstrCopyTo) == '/' ) {
      strcpy(strPathTo, "");
   }
   else {
      strcpy(strPathTo, g_strCurrentPath);
   }
   strcat(strPathTo, pstrCopyTo);

   FILE *fpRead = fopen(strPathFrom, "r");
   if( !fpRead ) {
      return HIGHSCORE_READ_ERROR;
   }

   FILE *fpWrite = fopen(strPathTo, "w");
   if( !fpWrite ) {
      return HIGHSCORE_WRITE_ERROR;
   }

   int nRet = HIGHSCORE_OK;
   char buffer[256];
   while(1) {
      int nRead = fread(buffer, 1, 256, fpRead);
      if( nRead <= 0 )
         break;
      int nWrite = fwrite(buffer, 1, nRead, fpWrite);
      if( nRead != nWrite ) {
         nRet = HIGHSCORE_WRITE_ERROR;
         break;
      }
   }

   fclose(fpRead);
   fclose(fpWrite);

   return nRet;
}

int HSDeleteFile(const char* pstrFile)
{
   DEBUG_FUNC_NAME;

   if( pstrFile == NULL ) {
      return HIGHSCORE_BADARGUMENT;
   }

   char strPath[MAX_PATH];

   //Does it start with a '/'; if not think of it as a relative path
   if( (*pstrFile) == '/' ) {
   }
   else {
      strcpy(strPath, g_strCurrentPath);
   }
   strcat(strPath, pstrFile);

   return remove(strPath) == 0 ? HIGHSCORE_OK : HIGHSCORE_UNABLE_DELETE_FILE;
}

// Category Related Section ///////////////////////////
const char* GetCategoryID(HighScoreLib api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   return pHS->m_strCurrentCategory;
}

int SetCategoryID(HighScoreLib api, Identifier ID)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   strcpy(pHS->m_strCurrentCategory, ID);
   return HIGHSCORE_OK;
}

int ClearCurrentCategoryID(HighScoreLib api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   strcpy(pHS->m_strCurrentCategory, "");
   return HIGHSCORE_OK;
}

// High Score Related Section /////////////////////////

Identifier FixIdentifier(struct HighScore* pHS, Identifier ID)
{
   DEBUG_FUNC_NAME;

   if( ID == NULL ) {
      if( strlen(pHS->m_strCurrentCategory) != 0 ) {
         return pHS->m_strCurrentCategory;
      }
      return "";
   }

   return ID;
}

int GetNumberScores(HighScoreLib api, Identifier ID)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, 
       FixIdentifier(pHS, ID));
   if( pHST == NULL ) {
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return 0;
   }

   int nRet = GetNumberOfScores(pHST);

   return nRet;
}

int GetLimitNumber(HighScoreLib api)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   return pHS->m_nLimitItems;
}

int SetLimitNumber(HighScoreLib api, int nAmount)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;
   
   pHS->m_nLimitItems = nAmount;

   if( nAmount == HIGHSCORE_NO_LIMIT )
      return HIGHSCORE_OK;//No reason to do clamping; no limit

   SetTableLimit(pHS->m_pRootHighScoreTables, nAmount);

   if( pHS->m_nBatch ) {
      return HIGHSCORE_BATCHING_OK;
   }

   return Persist(pHS);
}

int IsHighScore(HighScoreLib api, Identifier ID, int nScore)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   if( pHS->m_nLimitItems == HIGHSCORE_NO_LIMIT )
      return 1;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return HIGHSCORE_TABLE_NOT_CREATED;
   }

   return IsHighScoreForTable(pHST, nScore, pHS->m_nLimitItems);
}

int AddHighScore(HighScoreLib api, Identifier ID, int nScore, const char* pstrName, int nNumber, int* pnIndex, const char* pstrExtra)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) {
      pHST = CreateHighScoreTable(&pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   }

   if( pHST == NULL ) { //If still NULL; ran out of memory
      pHS->m_nLastError = HIGHSCORE_OUT_OF_MEMORY;
      return HIGHSCORE_OUT_OF_MEMORY;
   }

   if( !IsHighScore(api, ID, nScore) )
      return HIGHSCORE_NOT_HIGH_ENOUGH;

   int nRet = AddHighScoreEntry(pHST, nScore, pstrName, nNumber, pnIndex, pstrExtra != NULL ? pstrExtra : "");

   if( nRet != HIGHSCORE_OK )
      return nRet;

   if( pHS->m_nBatch ) {
      return HIGHSCORE_BATCHING_OK;
   }

   return Persist(pHS);
}

int ClearAllItems(HighScoreLib api, Identifier ID)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return HIGHSCORE_TABLE_NOT_CREATED;
   }

   int nRet = ClearAllEntries(pHST);

   if( nRet != HIGHSCORE_OK )
      return nRet;

   if( pHS->m_nBatch ) {
      return HIGHSCORE_BATCHING_OK;
   }

   return Persist(pHS);
}

int RemoveItem(HighScoreLib api, Identifier ID, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return HIGHSCORE_TABLE_NOT_CREATED;
   }

   int nRet = RemoveScoreIndex(pHST, nIndex);

   if( nRet != HIGHSCORE_OK )
      return nRet;

   if( pHS->m_nBatch ) {
      return HIGHSCORE_BATCHING_OK;
   }

   return Persist(pHS);
}

int GetScore(HighScoreLib api, Identifier ID, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return HIGHSCORE_TABLE_NOT_CREATED;
   }

   return GetHighScore(pHST, nIndex);
}

const char* GetName(HighScoreLib api, Identifier ID, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return "";
   }

   return GetHighScoreName(pHST, nIndex);
}

int GetNumber(HighScoreLib api, Identifier ID, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return 0;
   }

   return GetHighScoreNumber(pHST, nIndex);
}

const char* GetExtraData(HighScoreLib api, Identifier ID, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct HighScore* pHS = (struct HighScore*)api;

   struct HighScoreTable* pHST = GetHighScoreTable(pHS->m_pRootHighScoreTables, FixIdentifier(pHS, ID));
   if( pHST == NULL ) { //Doesn't exist
      pHS->m_nLastError = HIGHSCORE_TABLE_NOT_CREATED;
      return "";
   }

   return GetHighScoreExtraData(pHST, nIndex);
}

