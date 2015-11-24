#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



static const char *FindCharInStr(const char *pText, int iTextLen, int wch)
{
    const char *pRes = NULL;

    const char *p = pText;
    for (int i = 0; i < iTextLen; i++)
    {
        if (*p == wch)
        {
            pRes = p;
            break;
        }
        p++;
    }
    return pRes;
}




int UnescapePattern(const char *pText, int iTextLen)
{

    return ' ';


}




void UnescapeEntitiesForString(const char *pText, int iTextLen,
                               char *buffer, int buffer_len_max)
{

    int iCurPos = 0;
    int buffer_wr_pos = 0;


    while (iCurPos < iTextLen && buffer_wr_pos < buffer_len_max - 1)
    {

        int iNext;

        if (pText[iCurPos] != '&')      // not begin with '&'
        {
            // find next '&'
            const char *p = FindCharInStr(pText + iCurPos,
                                          iTextLen - iCurPos,
                                          '&');


            if (p == NULL)      // if not found
            {
                // copy the left string
                //strDes.append(pText + iCurPos, iTextLen - iCurPos);
                iNext = iTextLen;
            } else              // found '&'
            {
                // copy the content between here and '&',
                // and move to '&'
                iNext = (int) (p - pText);
                //      strDes.append(pText + iCurPos, iNext - iCurPos);
            }



        } else                  // begin with '&'
        {
            // try to find ';'
            const char *q =
                FindCharInStr(pText + iCurPos, iTextLen - iCurPos, ';');

            if (q == NULL)      // if not found
            {
                // copy the left string
                //strDes.append(pText+iCurPos, iTextLen-iCurPos);
                //iCurPos = iTextLen;
                iNext = iTextLen;
            } else              // found &...; pattern
            {
                int iEnd = (int) (q - pText);
                char wch = (char) UnescapePattern(pText + iCurPos,
                                                  iEnd - iCurPos + 1);
                if (wch != '\0')
                {
                    // succeeded, add it
                    buffer[buffer_wr_pos] = wch;
                    iCurPos = iEnd + 1;
                    buffer_wr_pos += 1;
                } else
                {
                    // failed, copy &, move next
                    buffer[buffer_wr_pos] = pText[iCurPos];
                    iCurPos++;
                    buffer_wr_pos += 1;
                }

                continue;       // process entity , goto next.

            }


        }

        int len = iNext - iCurPos;

        if (buffer_wr_pos + len > buffer_len_max - 1)
            len = buffer_len_max - buffer_wr_pos - 1;

        /* move may overlap memory area. */
        memmove(buffer + buffer_wr_pos, pText + iCurPos, len);

        iCurPos = iNext;
        buffer_wr_pos += len;



    }

    buffer[buffer_wr_pos] = '\0';

}
