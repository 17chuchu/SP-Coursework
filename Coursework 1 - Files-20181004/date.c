//
// Created by Chutmongkon Chuasaard on 04/10/2018.
//

#include "date.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


struct date {
    int day;
    int month;
    int year;
};

Date *date_create(char *datestr){
    char dateref [11];
    strcpy(dateref,datestr);

    char *sep = "/";
    int isvalid = 1;
    //Calendar today = Calendar.getInstance().get(Calendar.YEAR);

    //day
    int day = atoi (strtok(dateref, sep));
    //month
    int month = atoi (strtok(NULL, sep));
    //year
    int year = atoi (strtok(NULL, sep));

    if(2018 < year || year < 0)
    {
        isvalid = 0;
    }
    else if(month < 0 || 12 < month)
    {
        isvalid = 0;
    }
    else if(day < 1)
    {
        isvalid = 0;
    }
    else if(day > 30 && (month == 4 || month == 6 || month == 9 || month == 11))
    {
        isvalid = 0;
    }
    else if(day > 31 && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12))
    {
        isvalid = 0;
    }
    else if(day > 29 && month == 2 && year % 4 == 0)
    {
        isvalid = 0;
    }
    else if(day > 28 && month == 2)
    {
        isvalid = 0;
    }

    if(isvalid)
    {
        Date * result = (Date *)malloc(sizeof(Date));
        result->day = day;
        result->month = month;
        result->year = year;
        //printf("Created date %d/%d/%d\n",day,month,year);
        return result;
    }
    else
    {
        //printf("Failed to create Date %s\n",datestr);
        return NULL;
    }
}

Date *date_duplicate(Date *d) {
    if(d == NULL)
    {
        return NULL;
    }
    Date * result = malloc(sizeof(Date));
    result->day = d->day;
    result->month = d->month;
    result->year = d->year;
    return result;
}

int date_compare(Date *date1, Date *date2) {
    
    if(date1 == NULL && date2 != NULL){
        return -1;
    }
    else if(date1 != NULL && date2 == NULL)
    {
        return 1;
    }
    else if(date1 == NULL && date2 == NULL){
        return 0;
    }
    
    int result = 0;
    
    result += (date1->year - date2->year) * 365;
    result += (date1->month - date2->month) * 31;
    result += (date1->day - date2->day);
    
    return result;
}

void date_destroy(Date * d)
{
    if(d != NULL)
    {
        //printf("Free date %d/%d/%d\n",d->day,d->month,d->year);
        free(d);
        //printf("Free Complete date %d/%d/%d\n\n",d->day,d->month,d->year);
    }
    return;
}
