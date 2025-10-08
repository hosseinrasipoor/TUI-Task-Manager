#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
//////////////////////////// تعریف استراکت هامون و تعریف گلوبال تسک
struct Subtask
{
    int value;
    char sub_title[50];
};
struct Category
{
    char cat_title[50];
};
struct Task
{
    char title[50];
    int donevalue;
    struct Subtask sub[5];
    int prefer,categories_count,subtask_count;
    struct Category categories[5];
    int day,month,year;
    char description[200];
};

struct Task tasks[20];
enum CON {TASK,DESCRIPTION,DEADLINE,SUB,CATEGORY};

int valid(int y,int m,int d)// چک کردن فرمت روز ماه سال
{
    if(m>12 || m<1 || d>30 || d<1 || y<0)
        return 0;
    return 1;
}
void swap(struct Task *a,struct Task *b) // تابع جابه جا کردن دوتا تسک
{
    struct Task temp=*a;
    *a=*b;
    *b=temp;
}

void save_tasks_to_json(struct Task tasks[], int task_count)  // تابع سیو در فایل
{
    cJSON *tasks_json = cJSON_CreateArray(); // یه ارایه از تسکا

    for (int i = 0; i < task_count; i++) 
    {
        cJSON *task_json = cJSON_CreateObject(); // یه تسک جدید

        // ایتم های تسکو بهش میدیم
        cJSON_AddStringToObject(task_json, "title", tasks[i].title);
        cJSON_AddNumberToObject(task_json, "prefer", tasks[i].prefer);
        cJSON_AddNumberToObject(task_json, "donevalue", tasks[i].donevalue);
        cJSON_AddStringToObject(task_json, "description", tasks[i].description);
        cJSON_AddNumberToObject(task_json, "day", tasks[i].day);
        cJSON_AddNumberToObject(task_json, "month", tasks[i].month);
        cJSON_AddNumberToObject(task_json, "year", tasks[i].year);

        // ساخت ارایه ساب
        cJSON *subtasks = cJSON_CreateArray();
        for (int j = 0; j < tasks[i].subtask_count; j++) {
            cJSON *subtask = cJSON_CreateObject();
            cJSON_AddStringToObject(subtask, "sub_title", tasks[i].sub[j].sub_title);
            cJSON_AddNumberToObject(subtask, "value", tasks[i].sub[j].value);
            cJSON_AddItemToArray(subtasks, subtask);
        }
        cJSON_AddItemToObject(task_json, "subtasks", subtasks); // ساب به تسک اضافه میکنیم


        cJSON *categories = cJSON_CreateArray(); // ارایه کتگوری ها
        for (int j = 0; j < tasks[i].categories_count; j++) {
            cJSON *category = cJSON_CreateString(tasks[i].categories[j].cat_title); // اضافه کردن تایتل کتگوری
            cJSON_AddItemToArray(categories, category);  // اضافه کردن کتگوری به ارایه کتگوری ها
        }
        cJSON_AddItemToObject(task_json, "categories", categories); // اضافه کردن ارایه کتگوری ها به تسک

        // افزودن تسک به ارایه کل تسکا
        cJSON_AddItemToArray(tasks_json, task_json); 
    }

    char *json_string = cJSON_Print(tasks_json); // تبدیل کل ارایه تسکا به رشته برای ذخیره
    FILE *file = fopen("tasks.json", "w"); //ریختنش تو فایل
    if (file != NULL) { // اگه فایل خالی نبود بیاد قبلیارم بگیره همرو یجا بنویسه
        fputs(json_string, file);
        fclose(file);
    }

    free(json_string);
    cJSON_Delete(tasks_json);
}

void load_tasks_from_json(struct Task tasks[], int *task_count) 
{
    FILE *file = fopen("tasks.json", "r"); // باز کردن فایل و اگه فایل نبود ارور بده
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    fseek(file, 0, SEEK_END); // بردن کرسر ته فایل
    long length = ftell(file);// برگرداندن طول انتهای فایل که میشه اندازه کل رشته
    fseek(file, 0, SEEK_SET); // برگرداندن به اول فایل برای خوندن
    char *json_string = malloc(length + 1); // تعریف کردن جا برای جیسونمون
    fread(json_string, 1, length, file); // نوشتن فایل تو رشته جیسونمون
    json_string[length] = '\0';

    fclose(file);

    cJSON *tasks_json = cJSON_Parse(json_string); // تعریف ارایه رشته ها به صورت جیسون
    if (tasks_json == NULL) {
        printf("Error parsing JSON\n");
        free(json_string);
        return;
    }

    *task_count = cJSON_GetArraySize(tasks_json); // مقدار دهی تعداد تسکا
    for (int i = 0; i < *task_count; i++) 
    {
        cJSON *task_json = cJSON_GetArrayItem(tasks_json, i); // مقدار دادن به تک تک تسک ها
        strcpy(tasks[i].title, cJSON_GetObjectItem(task_json, "title")->valuestring);
        tasks[i].prefer = cJSON_GetObjectItem(task_json, "prefer")->valueint;
        tasks[i].donevalue = cJSON_GetObjectItem(task_json, "donevalue")->valueint;
        strcpy(tasks[i].description, cJSON_GetObjectItem(task_json, "description")->valuestring);
        tasks[i].day = cJSON_GetObjectItem(task_json, "day")->valueint;
        tasks[i].month = cJSON_GetObjectItem(task_json, "month")->valueint;
        tasks[i].year = cJSON_GetObjectItem(task_json, "year")->valueint;

        
        cJSON *subtasks = cJSON_GetObjectItem(task_json, "subtasks"); // مقدار دادن ساب تسک ها
        cJSON *subtask = NULL;
        int j = 0;
        cJSON_ArrayForEach(subtask, subtasks) // پیمایش تک تک ساب ها تا وقتی تموم شه
        {
            strcpy(tasks[i].sub[j].sub_title, cJSON_GetObjectItem(subtask, "sub_title")->valuestring);
            tasks[i].sub[j].value = cJSON_GetObjectItem(subtask, "value")->valueint;
            j++;
        }
        tasks[i].subtask_count = j;

        cJSON *categories = cJSON_GetObjectItem(task_json, "categories"); // مثل ساب
        cJSON *category = NULL;
        int k = 0;
        cJSON_ArrayForEach(category, categories) 
        {
            strcpy(tasks[i].categories[k].cat_title, category->valuestring);
            k++;
        }
        tasks[i].categories_count = k;
    }

    // پاک کردن حافظه مصرف شده
    free(json_string);
    cJSON_Delete(tasks_json);
}



int main(int argc, char *argv[])// دیدن ورودی که اگه اون دستور اجرا شد
{
    enum CON con=TASK;  // وضعیت درست کردن
    int task_count=0,task_loc=0,sub_loc=0,cat_loc=0; // ذخیره موقعیت فعلیمون در تسک و کتگوری و ساب تسک ها و تعداد تسک ها
    load_tasks_from_json(tasks,&task_count);
    if (argc > 1) // دستور کنسول چک شه
    {
        if (strcmp(argv[1], "list") == 0) 
        {
            for (int i = 0; i < task_count; i++) 
            {
                printf("%d. [%s] %s\n", i + 1, tasks[i].donevalue ? "X" : " ", tasks[i].title);
            }
            return 0;
        }
    }
    initscr();
    noecho();
    cbreak(); 
    keypad(stdscr, TRUE); 
    curs_set(0);
    int rows, cols;
    getmaxyx(stdscr, rows, cols);// گرفتن اندازه صفحه

            // تعریف صفحه ها
    WINDOW *task_win = newwin(rows * 0.8 , cols / 2, 0, 0);
    WINDOW *subtask_win = newwin(rows * 0.2, cols / 2, (rows *0.8), 0);
    WINDOW *description_win = newwin(rows *0.6, cols / 2, 0, (cols / 2) );
    WINDOW *indescription_win = newwin((rows *0.6)-2, (cols / 2)-2, 1, (cols / 2)+1 );
    WINDOW *category_win = newwin(rows* 0.2 , cols / 2, rows*0.6 , cols / 2 );
    WINDOW *deadline_win = newwin(rows *0.2, cols / 2, rows*0.8 , cols / 2 );
    
    
    void page()// یه تابع تعریف کردم که چند بار استفاده کنیم خواستیم
    {
        box(task_win, 0, 0);
        box(subtask_win,0,0);
        box(description_win, 0, 0);
        box(category_win, 0, 0);
        box(deadline_win, 0, 0);
        mvwprintw(task_win,0,1,"Tasks");
        mvwprintw(subtask_win,0,1,"Sub-Tasks");
        mvwprintw(description_win,0,1,"Description");
        mvwprintw(category_win,0,1,"Categories");
        mvwprintw(deadline_win,0,1,"Deadline");
    }

    void clear_win()
    {
        wclear(task_win);
        wclear(subtask_win);
        wclear(description_win);
        wclear(deadline_win);
        wclear(category_win);
    }

    void refresh_win()
    {
        refresh();
        wrefresh(task_win);
        wrefresh(subtask_win);
        wrefresh(description_win);
        wrefresh(category_win);
        wrefresh(deadline_win);
    }
    
    while(1) /// برنامه اجرا شه
    {
        curs_set(0);
        noecho();
        clear_win();
        page();
        refresh_win();
        ///////////////////////////////////// قسمت پرینت کردن همه چیز
        for(int i=0;i<task_count;i++)
        {
            if(task_loc==i)
            {
                wattron(task_win,A_REVERSE);
                mvwprintw(task_win,i+1,1,">>");
            }
            char conditon;
            if(tasks[i].donevalue==1)
                conditon='X';
            else if(tasks[i].donevalue==0)
                conditon=' ';
            mvwprintw(task_win,i+1,3,"%d.[%c] %s",tasks[i].prefer,conditon,tasks[i].title);// پرینت تسک ها
            wattroff(task_win,A_REVERSE);
        }

        if(task_count>0)
        {
            mvwprintw(indescription_win,0,0,"%s",tasks[task_loc].description);// پرینت دیسکریپشنی که روشیم
            mvwprintw(deadline_win,1,1,"%04d/%02d/%02d",tasks[task_loc].year,tasks[task_loc].month,tasks[task_loc].day); // دد لاینی که روشیم

            for(int j=0;j<tasks[task_loc].categories_count;j++)// پرینت کتگوری ها
            {
                if(con==CATEGORY && cat_loc==j)
                {
                    wattron(category_win,A_REVERSE);
                }
                mvwprintw(category_win,j+1,1,"%s",tasks[task_loc].categories[j].cat_title);
                wattroff(category_win,A_REVERSE);
            }
            for(int j=0;j<tasks[task_loc].subtask_count;j++) // پرینت ساب تسکا
            {
                if(con==SUB && sub_loc==j)
                {
                    wattron(subtask_win,A_REVERSE);
                }
                char conditon;
                if(tasks[task_loc].sub[j].value==1)
                    conditon='X';
                else if(tasks[task_loc].sub[j].value==0)
                    conditon=' ';
                mvwprintw(subtask_win,j+1,1,"[%c] %s",conditon,tasks[task_loc].sub[j].sub_title);
                wattroff(subtask_win,A_REVERSE);
            }
        }

        // اعمال تغیرات
        wrefresh(task_win);
        wrefresh(subtask_win);
        wrefresh(indescription_win);
        wrefresh(category_win);
        wrefresh(deadline_win);

        int c = getch(); // گرفتن دستور انجام کار ها

        if(c=='j') // پایین رفتن
        {
            if(con==TASK && task_loc<task_count-1)
            {
                task_loc++;
                sub_loc=0;
                cat_loc=0;
            }
            else if(con==SUB && sub_loc<tasks[task_loc].subtask_count-1)
                sub_loc++;
            else if(con==CATEGORY && cat_loc<tasks[task_loc].categories_count-1)
                cat_loc++;
            clear_win();
            refresh_win();
        }
        else if(c=='k') // بالا رفتن
        {
            if(con==TASK && task_loc>=0)
            {
                if(task_loc!=0)
                    task_loc--;
                sub_loc=0;
                cat_loc=0;
            }
            
            else if(con==SUB && sub_loc>0)
                sub_loc--;
            else if(con==CATEGORY && cat_loc>0)
                cat_loc--;
        
            clear_win();
            refresh_win();
        }
        else if(c=='l') // رفتن به ساب
        {
            con=SUB;
            if(tasks[task_loc].subtask_count==0)
            {
                mvwprintw(subtask_win,1,1,"there is no subtask availible!");
            }
        }
        else if(c=='h') // بازشگت به لیست اصلی
        {
            con=TASK;
            //clear_win();
            refresh_win();
        }
        else if(c=='a') // اضافه کردن ایتم جدید
        {
            curs_set(1);
            echo();
            if(con==TASK) // اضافه کرن تسک جدید
            {
                if(task_count==20)
                {
                    mvwprintw(task_win,21,1,"task count is full max 20!");
                    continue;
                }
                ////////////////////////////////// تایتل تسک
                char temp_string[50];
                werase(task_win);
                box(task_win, 0, 0);
                mvwprintw(task_win,0,1,"Input-Task");
                
                mvwprintw(task_win,1,1,"Enter your title: ");
                wgetnstr(task_win, temp_string, 49);
                strcpy(tasks[task_count].title,temp_string);
                ////////////////////////////////////// توضیحات تسک
                char temp_des[200];
                werase(task_win);
                box(task_win, 0, 0);
                mvwprintw(task_win,0,1,"Input-description-of-Task");
                echo();
                mvwprintw(task_win,1,1,"Enter your description: ");
                wgetnstr(task_win, temp_des, 199);
                strcpy(tasks[task_count].description,temp_des);
                //////////////////////////////////////////// ددلاین تسک
                int day,month,year;
                do{
                werase(task_win);
                box(task_win, 0, 0);
                mvwprintw(task_win,0,1,"Input-Date-of-Task");
                wrefresh(task_win);
                
                mvwprintw(task_win,1,1,"Enter your deadline year: ");
                wscanw(task_win, "%d", &year);
                mvwprintw(task_win,2,1,"Enter your deadline month: ");
                wscanw(task_win, "%d", &month);
                mvwprintw(task_win,3,1,"Enter your deadline day: ");
                wscanw(task_win, "%d", &day);
                }
                while(!valid(year,month,day));
                tasks[task_count].year=year;
                tasks[task_count].month=month;
                tasks[task_count].day=day;
                /////////////////////////////// اولویت تسک
                do{
                werase(task_win);
                box(task_win, 0, 0);
                mvwprintw(task_win,0,1,"preference-of-Task");
                wrefresh(task_win);
                mvwprintw(task_win,1,1,"Enter your task preference(1-9): ");
                wscanw(task_win, "%d", &(tasks[task_count].prefer));
                }
                while(tasks[task_count].prefer<1 || tasks[task_count].prefer>9);
                ////////////////////////////////////////////////
                
                werase(task_win); // اعمال تغیرات
                wrefresh(task_win);
                tasks[task_count].donevalue=0; // پیشفرض انجام نشده هست
                task_count++;
                
            }
            else if(con==SUB && task_count>0) // اضافه کردن ساب تسک
            {
                //werase(subtask_win);
                //wrefresh(subtask_win);
                wclear(subtask_win);
                box(subtask_win,0,0);
                if(tasks[task_loc].subtask_count==5) // پر شدن فضای ساب
                {

                    mvwprintw(deadline_win,5,1,"there is no free space for add sub tasks!(max is 5)");
                }
                else
                {
                    mvwprintw(subtask_win,0,1,"add sub task");
                    wrefresh(subtask_win);
                    mvwprintw(subtask_win,tasks[task_loc].subtask_count+1,1,"Enter your sub task %d: ",tasks[task_loc].subtask_count+1);
                    wgetnstr(subtask_win, tasks[task_loc].sub[tasks[task_loc].subtask_count].sub_title, 49);
                    tasks[task_loc].subtask_count++;
                    clear_win();
                    refresh_win();
                }
                

            }
            else if(con==CATEGORY && task_count>0) // اضافه کردن کتگوری
            {
                werase(category_win);
                wrefresh(category_win);
                box(category_win,0,0);
                if(tasks[task_loc].categories_count==5) // پر بودن کتگوری ها
                {
                    mvwprintw(deadline_win,5,1,"there is no free space for add categoties!(max is 5)");
                }
                else
                {
                    mvwprintw(category_win,0,1,"add category");
                    wrefresh(category_win);
                    mvwprintw(category_win,tasks[task_loc].categories_count+1,1,"Enter your category number %d: ",tasks[task_loc].categories_count+1);
                    wgetnstr(category_win, tasks[task_loc].categories[tasks[task_loc].categories_count].cat_title, 49);
                    tasks[task_loc].categories_count++;
                }
                
                clear_win();
                refresh_win();
            }
            else // شرط نبود تسک ولی میخواد اضافه کنه به اجزای تسک
            {
                mvwprintw(task_win,1,1,"you havent entered a task yet");
            }
            
            
            curs_set(0);
            noecho();

        }
        else if(c=='d') // حذف
        {
            if(con==TASK) // میاد اون عنصر رو میبره اخرین خونه تسک ها و تعداد تسکارو یکی کم میکنه
            {
                for(int i=task_loc;i<task_count-1;i++)
                {
                    swap(&tasks[i],&tasks[i+1]);
                }
                if(task_count>0) // اگر تعدادش صفر بود دیگه نباید کم کنه
                {
                    tasks[task_loc].subtask_count=0; // ریست کردن مکان ساب و کتگوری
                    tasks[task_loc].categories_count=0;
                    task_count--;
                    if(task_loc>0)
                        task_loc--;
                    
                }
                //clear_win();
                //refresh_win();
                wclear(category_win);
                clear_win();
                wclear(deadline_win);
                wclear(task_win);
                wrefresh(task_win);
            }
            else if(con==SUB) // این هم مثل بالایی با تفاوت اینکه ساب تسک 
            {
                for(int i=sub_loc;i<tasks[task_loc].subtask_count-1;i++)
                {
                    struct Subtask temp=tasks[task_loc].sub[i];
                    tasks[task_loc].sub[i]=tasks[task_loc].sub[i+1];
                    tasks[task_loc].sub[i+1]=temp;
                }
                if(tasks[task_loc].subtask_count>0)
                {
                    tasks[task_loc].subtask_count--;
                    sub_loc--;
                }
                    
                wclear(subtask_win);
                wrefresh(subtask_win);
            }
            else if(con==CATEGORY) // این هم مثل بالایی ولی کتگوری
            {
                for(int i=cat_loc;i<tasks[task_loc].categories_count-1;i++)
                {
                    struct Category temp=tasks[task_loc].categories[i];
                    tasks[task_loc].categories[i]=tasks[task_loc].categories[i+1];
                    tasks[task_loc].categories[i+1]=temp;
                }
                if(tasks[task_loc].categories_count>0)
                {
                    tasks[task_loc].categories_count--;
                    cat_loc--;
                }
                    
                wclear(category_win);
                wrefresh(category_win);
            }
        }
        else if(c=='e' && con==TASK) // تابع ادیت
        {
            int ch, pos = strlen(tasks[task_loc].title); // طول تایتلمون
            box(task_win, 0, 0);
            mvwprintw(task_win,0,1,"Edit-Task");
            move(task_loc+1, 9 + pos);
            wattron(task_win,A_REVERSE);
            curs_set(1);
            echo();
            while ((ch = getch()) != '\n')  // این برای اینکه تا وقتی اینتر نزده بتونه ادیت کنه
            {
                wrefresh(task_win);
                switch (ch) 
                {
                    case KEY_BACKSPACE:
                        if (pos > 0) // برای اینکه پاک کنه اون حرف اخرو
                        {
                            tasks[task_loc].title[--pos] = '\0';
                            wattron(task_win,A_REVERSE);
                            mvwprintw(task_win,task_loc+1,9, "%s", tasks[task_loc].title);
                            wattroff(task_win,A_REVERSE);
                            mvwprintw(task_win,task_loc+1,9+pos, " "); 
                            move(task_loc+1, 9 + pos);
                            wrefresh(task_win);
                        }
                        break;
                    default: // اگر حرفی وارد شد بیاد توی ارایه تایتل
                        if (pos < sizeof(tasks[task_loc].title) - 1 && ch >= 32 && ch <= 126) { 
                            tasks[task_loc].title[pos++] = ch;
                            tasks[task_loc].title[pos] = '\0';
                            mvwprintw(task_win,task_loc+1, 9, "%s", tasks[task_loc].title);
                            move(task_loc+1, 9 + pos);
                            wrefresh(task_win);
                        }
                        break;
                }
            }
            wattroff(task_win,A_REVERSE);
        }
        else if(c=='r') // یه دیسکریپشن جدید براش تعریف میکنه
        {
            wclear(indescription_win);
            char temp_des[200];
            curs_set(1);
            echo();
            mvwprintw(indescription_win,0,0,"Enter your new description: ");
            wmove(indescription_win,1,0);
            wgetnstr(indescription_win, temp_des, 199);
            strcpy(tasks[task_loc].description,temp_des);

        }
        else if(c=='n') // یه ددلاین جدید براش تعریف میکنه
        {
            curs_set(1);
            echo();
            int day,month,year;
                do{
                werase(deadline_win);
                box(deadline_win, 0, 0);
                mvwprintw(deadline_win,0,1,"Input-New-Date-of-Task");
                wrefresh(deadline_win);
                mvwprintw(deadline_win,1,1,"Enter your deadline year: ");
                wscanw(deadline_win, "%d", &year);
                mvwprintw(deadline_win,2,1,"Enter your deadline month: ");
                wscanw(deadline_win, "%d", &month);
                mvwprintw(deadline_win,3,1,"Enter your deadline day: ");
                wscanw(deadline_win, "%d", &day);
                }
                while(!valid(year,month,day)); // ورودی میگیره تا وقتی فرمت درست داده شه
                tasks[task_loc].year=year;
                tasks[task_loc].month=month;
                tasks[task_loc].day=day;
                wclear(deadline_win);
        }
        else if(c=='c') // میره به قسمت کتگوری
        {
            if(con==TASK)
            {
                con=CATEGORY;
                if(tasks[task_loc].categories_count==0)
                {
                    mvwprintw(category_win,1,1,"there is no category availible!");
                }
            }
            else if(con==CATEGORY)
                con=TASK;
        }
        else if(c==' ') // وضعیت انجام شدن تسک یا ساب تسک
        {
            if(con==TASK)
            {
                if(tasks[task_loc].donevalue==0)
                    tasks[task_loc].donevalue=1;
                else if(tasks[task_loc].donevalue==1)
                    tasks[task_loc].donevalue=0;
                wrefresh(task_win);
            }
            if(con==SUB)
            {
                if(tasks[task_loc].sub[sub_loc].value==0)
                    tasks[task_loc].sub[sub_loc].value=1;
                else if(tasks[task_loc].sub[sub_loc].value==1)
                    tasks[task_loc].sub[sub_loc].value=0;
                wrefresh(subtask_win);
            }
        }
        else if(c=='q') // تمام شدن برنامه
        {
            endwin();
            return 0;
        }
        else if(c=='w') // سیو کردن برنامه تو جیسون
        {
            save_tasks_to_json(tasks,task_count);
        }
        else if(c=='s' && task_count>0) // سورت کردن تسک ها برحسب چیزی که میخواد
        {
            werase(task_win);
            box(task_win, 0, 0);
            mvwprintw(task_win,0,1,"Sort-Task");
            mvwprintw(task_win,1,1,"Enter your type of sort(a,p,d)"); // نوع سورت شدن مشخص میشه
            curs_set(1);
            echo();
            char chars;
            while(1) // ورودی نوع سورت
            {
                wscanw(task_win,"%c",&chars);
                if(chars=='a') // به صورت بابل سورت میاد انجام میده
                {
                    for (int i = 0; i < task_count - 1; i++) 
                    {
                        for (int j = 0; j < task_count - i - 1; j++) 
                        {
                            if (strcmp(tasks[j].title,tasks[j+1].title) > 0) // تابع استفاده شده میاد میگه کدوم از نظر حروف الفبایی بزرگ تره
                            {
                                swap(&tasks[j], &tasks[j + 1]);
                            }
                        }
                    }   
                    break;
                }
                else if(chars=='p') // برحسب اون اولویت ها سورت میکنه
                {
                    for (int i = 0; i < task_count - 1; i++) 
                    {
                        for (int j = 0; j < task_count - i - 1; j++) 
                        {
                            if (tasks[j].prefer > tasks[j+1].prefer)
                            {
                                swap(&tasks[j], &tasks[j + 1]);
                            }
                        }
                    }
                    break;
                }
                else if(chars=='d') // از کم ارزش ترین که روز هست سورت میکنه بعد ماه بعد سال اینطوری کامل سورت شده هست
                {
                    for (int i = 0; i < task_count - 1; i++) 
                    {
                        for (int j = 0; j < task_count - i - 1; j++) 
                        {
                            if (tasks[j].day > tasks[j+1].day)
                            {
                                swap(&tasks[j], &tasks[j + 1]);
                            }
                        }
                    }
                    for (int i = 0; i < task_count - 1; i++) 
                    {
                        for (int j = 0; j < task_count - i - 1; j++) 
                        {
                            if (tasks[j].month > tasks[j+1].month)
                            {
                                swap(&tasks[j], &tasks[j + 1]);
                            }
                        }
                    }
                    for (int i = 0; i < task_count - 1; i++) 
                    {
                        for (int j = 0; j < task_count - i - 1; j++) 
                        {
                            if (tasks[j].year > tasks[j+1].year)
                            {
                                swap(&tasks[j], &tasks[j + 1]);
                            }
                        }
                    }
                    break;
                }
                else
                {
                    mvwprintw(task_win,2,1,"Enter valid char(a,p,d)");
                }
            }
            wclear(task_win);
        }
        else if(c=='y') //load
        {
            load_tasks_from_json(tasks,&task_count);
            clear_win();
        }
    }
    
    endwin();
    
}
