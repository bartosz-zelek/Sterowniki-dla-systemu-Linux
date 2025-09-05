/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "agent_UEFI.h"

#ifdef IS_UEFI

EFI_STATUS
clock_gettime(time_t *currentTime){
        EFI_TIME time;
        EFI_STATUS status;
        status = gRT->GetTime(&time, NULL);
        if (EFI_ERROR(status))
                return status;
        *currentTime = Efi2Time(&time);
        return status;
}

EFI_STATUS
clock_settime(struct timespec ts){
        EFI_TIME time;
        EFI_STATUS status;
        status = gRT->GetTime(&time, NULL);
        if (EFI_ERROR(status)) {
                return status;
        }
        struct tm *set_time = gmtime(&ts.tv_sec);

        time.Year = 1900 + set_time->tm_year;
        time.Month = set_time->tm_mon;
        time.Day = set_time->tm_mday;
        time.Hour = set_time->tm_hour;
        time.Minute = set_time->tm_min;
        time.Second = set_time->tm_sec;
        time.Nanosecond = set_time->tm_Nano;
        time.Daylight = set_time->tm_daylight;

        status = gRT->SetTime(&time);

        return status;
}

CHAR16*
convert_path_to_dos(const char *filePath){
        int offset = 0;
        if (*filePath == '/')
                offset = 1;

        int fileLength = AsciiStrLen(filePath);

        CHAR16 *w_result = AllocatePool(((AsciiStrLen(filePath) + 1 - offset) * sizeof(CHAR16)));
        mbstowcs(w_result, filePath + offset, AsciiStrLen(filePath) + 1 - offset);
        int i = 0;
        while (i++ < fileLength) {
                if (w_result[i] == '/')
                        w_result[i] = '\\';
        }

        return w_result;
}

char*
char16_to_char(CONST CHAR16 *source){
        char *result = AllocatePool(((StrLen(source) + 1) * sizeof(char)));
        wcstombs(result, source, StrLen(source) + 1);
        return result;
}


CONST CHAR16*
get_current_dir(){
        return ShellGetCurrentDir(NULL);
}

BOOLEAN
file_end_of_file(IN SHELL_FILE_HANDLE fileHandle){
        EFI_FILE_INFO *Info;
        UINT64 Pos;
        BOOLEAN RetVal;

        gEfiShellProtocol->GetFilePosition(fileHandle, &Pos);
        Info = gEfiShellProtocol->GetFileInfo(fileHandle);
        gEfiShellProtocol->SetFilePosition(fileHandle, Pos);

        if (Info == NULL)
                return (FALSE);
        if (Pos == Info->FileSize)
                RetVal = TRUE;
        else
                RetVal = FALSE;
        FreePool(Info);

        return (RetVal);
}

size_t
parse_directory(char **buffer, size_t atInput, CONST CHAR16 *fileName){
        CHAR16 *CorrectedPath = NULL;
        UINTN pathSize = 0;
        EFI_SHELL_FILE_INFO *curDirList = NULL;
        EFI_SHELL_FILE_INFO *childrenList = NULL;
        CONST EFI_SHELL_FILE_INFO *curNode = NULL;
        CONST EFI_SHELL_FILE_INFO *childNode = NULL;
        size_t result = atInput;

        StrnCatGrow(&CorrectedPath, &pathSize, fileName, 0);
        if (CorrectedPath[StrLen(CorrectedPath) - 1] != L'\\' && CorrectedPath[StrLen(CorrectedPath) - 1] != L'/')
                CorrectedPath = StrnCatGrow(&CorrectedPath, &pathSize, L"\\", 0);

        if (CorrectedPath == NULL)
                return result;

        ShellOpenFileMetaArg((CHAR16 *) CorrectedPath, EFI_FILE_MODE_READ, &curDirList);
        //clear some memory
        *CorrectedPath = CHAR_NULL;
        SHELL_FREE_NON_NULL(CorrectedPath);

        curNode = (EFI_SHELL_FILE_INFO *) GetFirstNode(&curDirList->Link);
        if (curNode == NULL)
                return result;

        EFI_STATUS status = gEfiShellProtocol->FindFilesInDir(curNode->Handle, &childrenList);
        if (EFI_ERROR(status)) {
                if (childrenList != NULL)
                        gEfiShellProtocol->FreeFileList(&childrenList);
                return result;
        }
        for (childNode = (EFI_SHELL_FILE_INFO *) GetFirstNode(&childrenList->Link);
                !IsNull(&childrenList->Link, &childNode->Link);
                childNode = (EFI_SHELL_FILE_INFO *) GetNextNode(&childrenList->Link, &childNode->Link)) {

                //skip . and ..
                if (StrCmp(childNode->FileName, L".") == 0 || StrCmp(childNode->FileName, L"..") == 0)
                        continue;

                char *nameChar = char16_to_char(childNode->FileName);
                result = dynstr_printf(buffer, result, "%s", nameChar);
                FreePool(nameChar);
                result++;
                if ((childNode->Info->Attribute & EFI_FILE_DIRECTORY) == EFI_FILE_DIRECTORY)
                        parse_directory(buffer, result, childNode->FullName);
        }

        if (childrenList != NULL)
                ShellCloseFileMetaArg(&childrenList);
        if (curDirList != NULL)
                ShellCloseFileMetaArg(&curDirList);

        return result;
}

int
uname(struct utsname *sys){
        sys->nodename = "UEFI";
        sys->sysname = "UEFI";
        sys->machine = "UEFI-machine";
        sys->release = "R";
        sys->version = "1.5";
        return 0;
}

BOOLEAN
path_is_root(CHAR16 *fullPath){
        BOOLEAN result = FALSE;
        //this is at root level
        int offset = (fullPath[StrLen(fullPath) - 1] == L'\\' || fullPath[StrLen(fullPath) - 1] == L'/') ? 2 : 1;
        if (fullPath[StrLen(fullPath) - offset] == L':')
                result = TRUE;

        return result;
}

BOOLEAN
path_is_dir(CHAR16 *fullPath){
        return wcsrchr((CONST CHAR16 *)fullPath, L'.') == NULL;
}

EFI_STATUS
create_folder_structure(CHAR16 *fullPath){
        EFI_STATUS status = EFI_SUCCESS;
        SHELL_FILE_HANDLE fileHandle = NULL;
        //skip root level
        if (path_is_root(fullPath))
                return EFI_SUCCESS;

        UINTN fileNameCopySize = 0;
        CHAR16 *FileNameCopy = NULL;
        StrnCatGrow(&FileNameCopy, &fileNameCopySize, fullPath, 0);

        if (FileNameCopy == NULL)
                return EFI_OUT_OF_RESOURCES;

        PathCleanUpDirectories(FileNameCopy);
        if (PathRemoveLastItem(FileNameCopy)) {
            //skip root level for dir type only
                if (path_is_root(FileNameCopy) && path_is_dir(fullPath)) {
                        status = ShellOpenFileByName(fullPath, &fileHandle, EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
                        if (!EFI_ERROR(status))
                                status = gEfiShellProtocol->CloseFile(fileHandle);
                        return status;
                }

                //if the directory doesn't exist, then call the function again.
                if (ShellIsDirectory(FileNameCopy) != EFI_SUCCESS)
                        create_folder_structure(FileNameCopy);

                //call again for the current folder as well
                status = ShellOpenFileByName(FileNameCopy, &fileHandle, EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
                if (!EFI_ERROR(status))
                        gEfiShellProtocol->CloseFile(fileHandle);
        }

        //special case for the last folder path.
        if (!path_is_root(fullPath) && fullPath[StrLen(fullPath) - 1] == L'/' && ShellIsDirectory(fullPath) != EFI_SUCCESS) {
                status = ShellOpenFileByName(fullPath, &fileHandle, EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
                if (!EFI_ERROR(status))
                        gEfiShellProtocol->CloseFile(fileHandle);
        }

        *FileNameCopy = CHAR_NULL;
        SHELL_FREE_NON_NULL(FileNameCopy);
        return status;
}

const char*
get_command_from_run_binary(const char *cmdLine){
        // format is "cd \"/some/path\" && actual_cmd_line"
        char* checkchar;
        if ((checkchar = strchr(cmdLine, '&')) &&
            (checkchar[1] == '&') &&
            (checkchar[2] == ' ') &&
            (size_t)(checkchar - cmdLine) + 4 < strlen(cmdLine) //skip the "&& " sequence and have at least one more char
            ) {
            return checkchar + 3; // skip "&& " sequence
        }
        return NULL;
}

const char*
get_dir_from_run_binary(const char *cmdLine, size_t * len){
        // format is "cd \"/some/path\" && actual_cmd_line"
        // we need to drop quotations. this will confuse UEFI otherwise
        char* first_ampersand = strchr(cmdLine, '&');

        // we need to strip 6 chars: leading "cd \"" (which is 4)
        // and trailing "\" " (which is 2)
        if ((size_t) first_ampersand > (size_t) cmdLine + 6){
            *len = ((size_t) (first_ampersand - cmdLine) - 6);
            return cmdLine + 4;
        }
        *len = 0;
        return NULL;
}

EFI_STATUS
create_handle_from_path(CHAR16 *binaryPath, OUT EFI_HANDLE *driverHandle){
        EFI_STATUS status = EFI_SUCCESS;
        EFI_DEVICE_PATH_PROTOCOL *devicePathProtocol = NULL;
        EFI_LOADED_IMAGE_PROTOCOL *driverImage = NULL;

        devicePathProtocol = gEfiShellProtocol->GetDevicePathFromFilePath(binaryPath);
        // Use LoadImage to get it into memory
        status = gBS->LoadImage(FALSE, gImageHandle, devicePathProtocol, NULL, 0, driverHandle);
        if (EFI_ERROR(status)) {
                Print(L"There was an error on loading the %ls . Error code %d\n",
                       binaryPath, status);
                return status;
        }

        status = gBS->HandleProtocol(*driverHandle, &gEfiLoadedImageProtocolGuid, (VOID *) &driverImage);
        if (EFI_ERROR(status)) {
                Print(L"There was an error creating the handle for %ls . Error code %d\n",
                       binaryPath, status);
                return status;
        }

        // clean up memory...
        if (devicePathProtocol != NULL)
                FreePool(devicePathProtocol);
        return status;
}

EFI_STATUS
wait_for_timer(UINT64 timeout){
        EFI_STATUS result = EFI_SUCCESS;
        EFI_EVENT timer = NULL;
        UINTN eventIndex;
        result = gBS->CreateEvent(EVT_TIMER,
                                  TPL_NOTIFY,
                                  NULL, //function to be called
                                  NULL,
                                  &timer);
        if (!EFI_ERROR(result)){
                result = gBS->SetTimer(timer,
                                       TimerRelative,
                                       EFI_TIMER_PERIOD_MILLISECONDS(timeout));
        }
        if (EFI_ERROR(result)){
                gBS->CloseEvent(timer);
                return result;
        }
        result = gBS->WaitForEvent(1, &timer, &eventIndex);
        if (timer != NULL)
                gBS->SetTimer(timer, TimerCancel, 0);

        // Close the timer event
        gBS->CloseEvent(timer);

        // If the timer event expired return EFI_TIMEOUT
        if (!EFI_ERROR(result) && eventIndex == 1)
                result = EFI_TIMEOUT;

        return result;
}

#endif
