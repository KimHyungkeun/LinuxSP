#include "ssu_backup.h"

int main(int argc, char* argv[]) {

   
   char mkdir_command[BUFFER_MAX];
   char logmake_command[BUFFER_MAX];

    strcpy(logfile, "log.txt");

    if(argc > 2) {
         fprintf(stderr, "Usage : %s [directory path]\n", argv[0]);
          exit(1);
    }

    if(argv[1] == NULL) {
         strcpy(backup_dir, "ssu_backup_dir");
         

         if(access(backup_dir, F_OK) == 0) 
                        ;
         

         else {
         sprintf(mkdir_command, "%s %s", "mkdir", backup_dir);
         system(mkdir_command);
         }
            
    }

    else{
           strcpy(backup_dir, argv[1]);
           sprintf(mkdir_command, "%s %s", "mkdir", backup_dir);
           system(mkdir_command);
    }

    //printf("%s\n", backup_dir);
    stat(backup_dir, &statbuf);

    if(access(backup_dir, F_OK) < 0) {
         fprintf(stderr, "Usage : %s [directory path]\n", argv[0]);
         exit(1);
    }

    if(!S_ISDIR(statbuf.st_mode)) {
         fprintf(stderr, "Usage : %s [directory path]\n", argv[0]);
         exit(1);
    }

    

   prompt_environment(); //프롬프트 
 
   exit(0);

}

void list_function(Backup_list* head) {

    Backup_list* listcurr;
    listcurr = head -> next;

    if(head -> next == NULL)
        return;

    while(listcurr != NULL) {
        printf("%s     %d\n", listcurr -> filepath, listcurr -> period);
        listcurr = listcurr -> next;
    }
    
}

void prompt_environment(void) {
    int idx = 0;
    struct tm *tm_p;
    time_t now;
    Backup_list *head;
    Backup_list *curr;
 
   
    if((log_fp = fopen(logfile, "w")) == NULL) {
        fprintf(stderr, "fopen error\n");
        return;
    }
    setbuf(log_fp, NULL);
    //fprintf(log_fp, "이건 써지나?");

    head = (Backup_list*)malloc(sizeof(Backup_list));
    curr = head;
    curr -> next = NULL;
    curr -> prev = NULL;

    while(1) {

        time(&now);
        tm_p = localtime(&now);
        fputs(prompt, stdout);

        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = 0;
        strcpy(command_tmp, command);

        if (strstr(command,"vi") != NULL) { //vi 명령
             system(command); //명령 실행
             continue;
        }

        else if (strstr(command,"ls") != NULL) { //list 명령
             system(command); //명령 실행
             continue;
        }
           

        else if (strstr(command,"add") != NULL) { //add 명령
            //printf("add is executed\n");
            if(strcmp(command,"add") == 0) {
                fprintf(stderr, "Usage : %s FILENAME [PERIOD] [OPTION]\n", command);
                continue;
            }
            if(add_command_analyzer(head) != 0) 
                continue;
            //fprintf(log_fp, "여기는?\n");
            curr -> next = (Backup_list*)malloc(sizeof(Backup_list));
            
            if(pthread_create(&curr -> next -> tid, NULL ,add_function, (void *)curr) != 0) {
                fprintf(stderr, "thread_create error\n");
                continue;
            }
            //printf("Main Thread : pid %u tid %u \n", (unsigned int)pid, (unsigned int)tid);
            //pthread_join(tid, (void*)&status);
            curr = curr -> next;
            continue;
        }

        else if (strstr(command,"remove") != NULL) { //remove 명령
             //printf("remove is executed\n");
             if(strcmp(command,"remove") == 0) {
                fprintf(stderr, "Usage : %s FILENAME [OPTION]\n", command);
                continue;
             }

             curr = remove_function(head);
             continue;
        }

        else if (strstr(command,"compare") != NULL) { //compare 명령
             //printf("compare is executed\n");
             if(strcmp(command,"compare") == 0) {
                 fprintf(stderr, "Usage : %s FILENAME1 FILENAME2\n", command);
                 continue;
             }
             compare_function();
             continue;
        }

        else if (strstr(command,"recover") != NULL) { //recover 명령
             printf("recover is executed\n");
             continue;
        }


        else if (strstr(command,"list") != NULL) { //list 명령
             //printf("list is executed\n"); //명령 실행
             list_function(head);
             continue;
        }

        else if (strstr(command,"exit") != NULL) { //프롬프트 종료 명령
            free(head);
            break;
        }

        else if (strcmp(command,"\0") == 0) {
            continue;
        }

        else {
            printf("Unknown Command\n");  //프롬프트 종료 명령
            continue;
        }

    }

    fclose(log_fp);
    //fprintf(stdout, "Bye~!\n");
    //fflush(stdout);
}


int add_command_analyzer(Backup_list* head) {

    char *ptr;
    char *command_token[10];
    char filename_buf[BUFFER_MAX];
    char period[4];
    int i = 0;
    int num;
    //Backup_list* listhead = head;
    Backup_list *curr;
    ptr = strtok(command, " ");

    while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    {
        command_token[i] = ptr;          // 자른 문자열 출력
        //printf("%s\n", command_token[i]);
        //printf("i : %d\n", i);
        i++;  
        
        ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
    }

    strcpy(filename, command_token[1]);
    stat(filename, &statbuf);
    if(i < 3) {
        fprintf(stderr,"Usage : %s FILENAME [PERIOD] [OPTION]\n",command_token[0]);
        return 1;
    }
 
    if(access(filename, F_OK) < 0) {
        fprintf(stderr,"\"%s\" does not exist\n", filename);
        return 1;
    }

    if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr,"Not regular file\n");
        return 1;
    }

    curr = head;
    while (curr != NULL) {
        if(strcmp(curr -> filepath, realpath(filename,filename_buf)) == 0 ) {
            fprintf(stderr,"\"%s\" is already exist in backup list.\n", filename);
            return 1;
        }
        curr = curr -> next;
    }

    strcpy(period, command_token[2]);
  
    

    if (strstr(period,".") != NULL) {
        fprintf(stderr,"%s is not Integer. Try again\n", period);
        return 1;
    }


    if(atoi(period) < 5 || atoi(period) > 10) {
        fprintf(stderr,"Please retry (5 <= PERIOD <= 10)\n");
        return 1;
    }


    return 0;

}

void *add_function(void *arg) {
    struct tm *tm_p;
    struct tm time_struct;
    time_t now;
    int i = 0;

    char buf[BUFFER_MAX];
    char log_buf[BUFFER_MAX];
    char bck_buf[BUFFER_MAX];
    char cp_command[500];
    char period[4];

    char *strrchr_ptr;
    char *ptr;
    char *command_token[10];

    char filename_local[BUFFER_MAX];
    char command_local[BUFFER_MAX];
    char backup_local[BUFFER_MAX];

    char backup_realpath[BUFFER_MAX];
    char filename_final[BUFFER_MAX];
  
    Backup_list* listcurr;
    listcurr = (Backup_list*)arg;


    strcpy(command_local, command_tmp);

    ptr = strtok(command_local, " ");
    while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    {
        command_token[i] = ptr;          // 자른 문자열 출력
        i++;  
        
        ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
    }

    strcpy(period, command_token[2]);
    strcpy(filename_local, filename);
    strcpy(backup_local, backup_dir);
    
    listcurr -> next -> prev = listcurr;
    listcurr -> next -> pid = getpid();
    //listcurr -> next -> tid = pthread_self();
    strcpy(listcurr -> next ->  filepath, realpath(filename_local,buf));
    listcurr -> next -> period = atoi(period);
    
    listcurr -> next -> next = NULL;

    strrchr_ptr = strrchr(filename_local, '/');

    if (strrchr_ptr != NULL)
        strcpy(filename_final, strrchr_ptr+1);
    else
        strcpy(filename_final, filename_local);
    
    time(&now);
    tm_p = localtime_r(&now, &time_struct);
    
    sprintf(bck_buf, "%s_%d%02d%02d%02d%02d%02d", filename_final,tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
    sprintf(log_buf,"[%d%02d%02d %02d%02d%02d] %s/%s_%d%02d%02d%02d%02d%02d added\n",tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec, realpath(backup_local, backup_realpath), filename_final,tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
    sprintf(cp_command, "%s %s %s/%s","cp",realpath(filename_local,buf),backup_dir,bck_buf);
    //printf("bck_buf : %s\n", bck_buf);
    //printf("[%s] %ld\n", log_buf, ftell(log_fp));
    fprintf(log_fp, "%s", log_buf);
    system(cp_command);
    sleep(atoi(period));
  
    while(1) {
        time(&now);
        tm_p = localtime_r(&now, &time_struct);

        sprintf(bck_buf, "%s_%d%02d%02d%02d%02d%02d", filename_final ,tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
        sprintf(log_buf,"[%d%02d%02d %02d%02d%02d] %s/%s_%d%02d%02d%02d%02d%02d generated\n",tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec, realpath(backup_local, backup_realpath), filename_final, tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
        sprintf(cp_command, "%s %s %s/%s","cp",realpath(filename_local,buf),backup_dir,bck_buf);
        //printf("tid : %lu\n", listcurr -> tid);
        fprintf(log_fp, "%s", log_buf);
        system(cp_command);
        sleep(atoi(period));
    }
     
    
}

Backup_list* remove_function(Backup_list* head) {

    char *strrchr_ptr;
    char *ptr;
    char *command_token[2];
    char buf[BUFFER_MAX];
    char backup_local[BUFFER_MAX];
    char backup_realpath[BUFFER_MAX];
    char filename_local[BUFFER_MAX];

    char filename_realpath[BUFFER_MAX];
    char token_realpath[BUFFER_MAX];

    char filename_final[BUFFER_MAX];

    int i = 0;
    struct tm *tm_p; 
    struct tm time_struct;
    time_t now;
    pthread_t local_tid;
    Backup_list *curr;

    strcpy(backup_local, backup_dir);
    strcpy(filename_local, filename);
    ptr = strtok(command, " ");

    while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    {
        command_token[i] = ptr;          // 자른 문자열 출력
        //printf("%s\n", command_token[i]);
        //printf("i : %d\n", i);
        i++;  
        ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
    }

    
    strrchr_ptr = strrchr(filename_local, '/');

    if (strrchr_ptr != NULL)
        strcpy(filename_final, strrchr_ptr+1);
    else
        strcpy(filename_final, filename_local);

    if(strcmp(command_token[1], "-a") == 0) {
        curr = head -> next;
        while (curr != NULL) { 
            time(&now);
            tm_p = localtime_r(&now, &time_struct);
            pthread_cancel(curr -> tid);
            //printf("tid :%u\n", (unsigned int)curr -> tid);
            curr -> prev -> next = curr -> next;
            if(curr -> next == NULL)
                curr -> next = NULL;
            else 
                curr -> next -> prev = curr -> prev;

            fprintf(log_fp, "[%d%02d%02d %02d%02d%02d] %s/%s_%d%02d%02d%02d%02d%02d deleted\n",tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec, realpath(backup_local, backup_realpath) ,filename_final,tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
            free(curr);
            curr = head -> next;
        }
        curr = head;
    }

    else {
        
        curr = head -> next;
        while (curr != NULL) {
            time(&now);
            tm_p = localtime_r(&now, &time_struct);
            
            if(strcmp(curr -> filepath, realpath(filename_local, buf)) == 0) {
                local_tid = curr -> tid;
                pthread_cancel(local_tid);
                curr -> prev -> next = curr -> next;
                if(curr -> next == NULL)
                            ;
                else 
                    curr -> next -> prev = curr -> prev;

                
                fprintf(log_fp, "[%d%02d%02d %02d%02d%02d] %s/%s_%d%02d%02d%02d%02d%02d deleted\n",tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec, realpath(backup_dir, backup_realpath), filename_final,tm_p -> tm_year - 100, tm_p -> tm_mon+1, tm_p -> tm_mday, tm_p -> tm_hour, tm_p -> tm_min, tm_p -> tm_sec);
                free(curr);

                curr = head;
                while(curr != NULL) {
                    
                    if(curr -> next == NULL) {
                    //printf("%s\n", curr -> filepath);
                    break;
                    }

                    curr = curr -> next;
                }
                
                return curr;
            }

            else
                curr = curr -> next;
        }
       
    }


    return curr;
}

void compare_function(void) {

    struct stat statbuf1;
    struct stat statbuf2;
    char *ptr;
    int i = 0;
    int exist_flag = 0;
    int regular_flag = 0;
    char* command_token[3];

    char filename1[BUFFER_MAX];
    char filename2[BUFFER_MAX];

    ptr = strtok(command," ");
    while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    {
        command_token[i] = ptr;          // 자른 문자열 출력
        //printf("%s\n", command_token[i]);
        //printf("i : %d\n", i);
        i++;  
        ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
    }

    if (i != 3) {
        fprintf(stderr, "Usage : %s FILENAME1 FILENAME2\n", command_token[0]);
        return;
    }


    strcpy(filename1, command_token[1]);
    strcpy(filename2, command_token[2]);
    stat(filename1, &statbuf1);
    stat(filename2, &statbuf2);


    if(access(filename1, F_OK) < 0 && access(filename2, F_OK) < 0) {
        fprintf(stderr, "\"%s\" does not exist\n", filename1);
        fprintf(stderr, "\"%s\" does not exist\n", filename2);
        exist_flag = 1;
    }

    else if(access(filename1, F_OK) < 0) {
        fprintf(stderr, "\"%s\" does not exist\n", filename1);
        exist_flag = 1;
    }

    else if(access(filename2, F_OK) < 0) {
        fprintf(stderr, "\"%s\" does not exist\n", filename2);
        exist_flag = 1;
    }

    if(exist_flag == 1)
        return;


    if(!S_ISREG(statbuf1.st_mode) && !S_ISREG(statbuf2.st_mode)) {
        fprintf(stderr, "\"%s\" is not regular file\n", filename1);
        fprintf(stderr, "\"%s\" is not regular file\n", filename2);
        regular_flag = 1;
    }

    else if(!S_ISREG(statbuf1.st_mode)) {
        fprintf(stderr, "\"%s\" is not regular file\n", filename1);
        regular_flag = 1;
    }

    else if(!S_ISREG(statbuf2.st_mode)) {
        fprintf(stderr, "\"%s\" is not regular file\n", filename2);
        regular_flag = 1;
    }

    if(regular_flag == 1)
        return;


    if((statbuf1.st_mtime == statbuf2.st_mtime) && (statbuf1.st_size == statbuf2.st_size)) {
        printf("\"%s\" and \"%s\" is same!\n", filename1, filename2);
        return;
    }

    else {
        printf("FILE1 : \"%s\" MTIME : %lu SIZE : %lu\n", filename1, statbuf1.st_mtime, statbuf1.st_size);
        printf("FILE2 : \"%s\" MTIME : %lu SIZE : %lu\n", filename2, statbuf2.st_mtime, statbuf2.st_size);
        return;
    }

}
