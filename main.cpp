// system includes
#include <cmath>
#include <cstdbool>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <sys/un.h>
#include <stddef.h>
#include <inttypes.h>

using namespace std;


#define TMP_BUFF_LEN 128


#define CFG_FILE        "./cfg.ini"
#define CFG_FILE_BAK    "./cfg_bak.ini"


/****************************************************************/
/****************************************************************/
/*******************   Tool Fuctions   **************************/
/****************************************************************/
/****************************************************************/
vector<string> split(const string& s, const string& delim)
{
    vector<string> elems;

    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();

    if (delim_len == 0)
    {
        return elems;
    }

    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }

    return elems;
}

char* del_left_trim(char *str)
{
    if (str == NULL)
    {
        return NULL;
    }
    else
    {
        for (; *str != '\0' && isblank(*str); ++str);
        return str;
    }
}

char* del_all_trim(char* str)
{
    char* p;
    char* szOutput;
    szOutput = del_left_trim(str);
    for (p = szOutput + strlen(szOutput) - 1; p >= szOutput && isblank(*p); --p);
    *(++p) = '\0';
    return szOutput;
}


void setCfg(char* item, char* value)
{
    /* limit the length of item and value */
    if ((item == NULL) || (value == NULL)
        || strlen(item) == 0 || strlen(item) > 50
        || strlen(value) == 0 || strlen(value) > 50)
    {
        printf("setCfg error input!\n");
        return;
    }

    /* limit the item */
    /*
    if((strncmp(item, CFG_ITEM_LIMIT, strlen(FC_CFG_ITEM_IR)) != 0)
    {
        printf("invalid item!\n");
        return;
    }
    */

    bool bHave = false;

    char buf[TMP_BUFF_LEN];
    char s[TMP_BUFF_LEN];

    vector<string> vector_cfg;
    char vector_item[TMP_BUFF_LEN];
    char vector_value[TMP_BUFF_LEN];

    char* delim = "=";
    char* p;

    char str[TMP_BUFF_LEN];
    FILE* fp_bak = NULL;
    FILE* fp = NULL;

    if ((fp_bak = fopen(CFG_FILE_BAK, "w+")) != NULL)
    {
        fp = fopen(CFG_FILE, "r");
        if (fp == NULL)
        {
            // Don't have cfg file before, so create a new one.
            memset(str, 0, sizeof(str));
            snprintf(str, sizeof(str), "%s=%s\n", item, value);
            fwrite(str, strlen(str), 1, fp_bak);
            fclose(fp_bak);
            rename(CFG_FILE_BAK, CFG_FILE);
            system("chmod 666 " CFG_FILE);  //chmod(CFG_FILE, 666);
        }
        else
        {
            while (!feof(fp) && (p = fgets(buf, sizeof(buf), fp)) != NULL )
            {
                memset(s, 0, sizeof(s));
                strncpy(s, p, strlen(p));
                char* left_trim_str = del_left_trim(s);
                if ((strlen(left_trim_str) >= 1) && (left_trim_str[strlen(left_trim_str)-1] == '\n'))
                {
                    left_trim_str[strlen(left_trim_str)-1] = '\0';
                }

                vector_cfg = split(left_trim_str, delim);

                if (vector_cfg.size() >= 2)
                {
                    memset(vector_item, 0, sizeof(vector_item));
                    memset(vector_value, 0, sizeof(vector_value));

                    strncpy(vector_item, vector_cfg[0].c_str(), strlen(vector_cfg[0].c_str()));
                    strncpy(vector_value, vector_cfg[1].c_str(), strlen(vector_cfg[1].c_str()));

                    char* blank_trim_item = del_all_trim(vector_item);
                    char* blank_trim_value = del_all_trim(vector_value);

                    memset(str, 0, sizeof(str));

                    if (strncmp(blank_trim_item, item, strlen(item)) == 0)  /*Replace the old value*/
                    {
                        bHave = true;
                        snprintf(str, sizeof(str), "%s=%s\n", blank_trim_item, value);
                        printf("UpdateCfg %s", str);
                    }
                    else    /*Keep the old item=value*/
                    {
                        snprintf(str, sizeof(str), "%s=%s\n", blank_trim_item, blank_trim_value);
                    }

                    fwrite(str, strlen(str), 1, fp_bak);
                }
                else
                {
                    fwrite(left_trim_str, strlen(left_trim_str), 1, fp_bak);
                }
            }

            if (!bHave) /*Did not replace, so add a new item&value*/
            {
                memset(str, 0, sizeof(str));
                snprintf(str, sizeof(str), "%s=%s\n", item, value);
                fwrite(str, strlen(str), 1, fp_bak);
            }

            fclose(fp_bak);
            fclose(fp);

            rename(CFG_FILE_BAK, CFG_FILE);
            system("chmod 666 " CFG_FILE);  //chmod(CFG_FILE, 666);
        }
    }
}

void getCfg(char* item, char *value)
{
    if ((item == NULL) || (value == NULL))
    {
        return;
    }

    FILE* fp = NULL;
    char buf[TMP_BUFF_LEN];
    char s[TMP_BUFF_LEN];

    vector<string> vector_cfg;
    char vector_item[TMP_BUFF_LEN];
    char vector_value[TMP_BUFF_LEN];

    char* delim = "=";
    char* p;

    if ((fp = fopen(CFG_FILE, "r")) != NULL)
    {
        while (!feof(fp) && (p = fgets(buf, sizeof(buf), fp)) != NULL )
        {
            memset(s, 0, sizeof(s));
            strncpy(s, p, strlen(p));
            char* left_trim_str = del_left_trim(s);
            if ((strlen(left_trim_str) >= 1) && (left_trim_str[strlen(left_trim_str)-1] == '\n'))
            {
                left_trim_str[strlen(left_trim_str)-1] = '\0';
            }

            if (left_trim_str[0] == '#' || isblank(left_trim_str[0]) || left_trim_str[0] == '\n')
            {
                continue;
            }

            vector_cfg = split(left_trim_str, delim);

            if (vector_cfg.size() >= 2)
            {
                memset(vector_item, 0, sizeof(vector_item));
                memset(vector_value, 0, sizeof(vector_value));

                strncpy(vector_item, vector_cfg[0].c_str(), strlen(vector_cfg[0].c_str()));
                strncpy(vector_value, vector_cfg[1].c_str(), strlen(vector_cfg[1].c_str()));

                char* blank_trim_item = del_all_trim(vector_item);
                char* blank_trim_value = del_all_trim(vector_value);

                if (strncmp(blank_trim_item, item, strlen(item)) == 0)
                {
                    strncpy(value, blank_trim_value, strlen(blank_trim_value));
                    fclose(fp);
                    return;
                }
            }
        }

        fclose(fp);
    }
}

int main(int argc, char* argv[])
{
    char value[TMP_BUFF_LEN];

    memset(value, 0, sizeof(value));
    getCfg("item1", value);
    printf("getCfg1 item1=%s\n", value);

    memset(value, 0, sizeof(value));
    snprintf(value, sizeof(value), "%s", "value1");
    setCfg("item1", value);

    memset(value, 0, sizeof(value));
    getCfg("item1", value);
    printf("getCfg2 item1=%s\n", value);

    memset(value, 0, sizeof(value));
    getCfg("item2", value);
    printf("getCfg item2=%s\n", value);

    memset(value, 0, sizeof(value));
    getCfg("item3", value);
    printf("getCfg item3=%s\n", value);

    return 0;
}

