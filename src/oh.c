#include "user.h"


/*
�̸�    : mycd �Լ�
�ۼ���  : ������
���    : �۾� ���丮�� ��θ� �����Ѵ�
�޴°�  : ��� ���ڿ�
���ϰ�  : X
*/
void mycd (char *path)
{
    DIR_LIST *tmp_delete_ptr; //���� ��带 ����ų ������
    DIR_LIST *tmp_delete_back_ptr; //���� ����� �� ��带 ����ų ������

    //��ΰ� �Էµ��� ���� ���(root�� �̵�)
    if((path == NULL) || (strcmp(path, "/") == 0) || ((*path) == 0))
    {
        //��� ���� �����ϱ�
        while(1)
        {
            tmp_delete_ptr = front_dir_list_ptr;
            tmp_delete_back_ptr = NULL;

            //���� ������ ���� �̵�
            while(tmp_delete_ptr->next_ptr != NULL)
            {
                tmp_delete_back_ptr = tmp_delete_ptr;
                tmp_delete_ptr = tmp_delete_ptr->next_ptr;
            }

            //��� ����, ����
            if(tmp_delete_back_ptr == NULL) //��尡 �ϳ� ���� ���(root�� ���)
            {
                tmp_delete_ptr->next_ptr = NULL;

                rear_dir_list_ptr = front_dir_list_ptr;

                return;
            }
            else
            {                
                tmp_delete_back_ptr->next_ptr = NULL;
                free(tmp_delete_ptr->name);
                free(tmp_delete_ptr);
            }
        }
    }
    
    //path ������ ��ó��(�� �� ���ڿ��� ���� Ȯ��)
    if(*path == '/') //�� ���� /�� ���
    {
        //��� ���� �����ϱ�(���� ��ġ /)
        while(1)
        {
            tmp_delete_ptr = front_dir_list_ptr;
            tmp_delete_back_ptr = NULL;

            //���� ������ ���� �̵�
            while(tmp_delete_ptr->next_ptr != NULL)
            {
                tmp_delete_back_ptr = tmp_delete_ptr;
                tmp_delete_ptr = tmp_delete_ptr->next_ptr;
            }

            //��� ����, ����
            if(tmp_delete_back_ptr == NULL) //��尡 �ϳ� ���� ���(root�� ���)
            {
                tmp_delete_ptr->next_ptr = NULL;

                rear_dir_list_ptr = front_dir_list_ptr;

                break;
            }
            else
            {
                tmp_delete_back_ptr->next_ptr = NULL;
                free(tmp_delete_ptr->name);
                free(tmp_delete_ptr);
            }
        }

        //path���� / �����ϱ�
        path++;
    }
    else if((*path == '.') && (*(path + 1) == '/')) //�� ���� .�� ��� (..�� �ƴϾ�� ��)
    {
        //��� �״�� ����ϱ�(���� ��ġ .)
        //path���� ./ �����ϱ�
        path += 2;
    }
    else // /�� ./ �� �� �ƴ� ��쿡�� path�� �״�� ���
    {
        ;
    }

    //ó���� path�� ��� �����ϱ� -> ..�̸� ��� �ϳ� ����, ..�� �ƴϸ� ��� �߰�
    FILE *myfs;
    myfs = fopen("myfs", "rb");

    INODE *i_data = (INODE *)malloc(sizeof(INODE)); //���� ���丮�� inode ������ ������ ����ü�� ����ų ������

    char *filename = (char *)malloc(sizeof(char) * 8);
    int *inodenumber = (int *)malloc(sizeof(int));
    
    int count = 0;
    int dir_file_num;

    char *nm_ptr = strtok(path, "/");
    do
    {
        if(strcmp(nm_ptr, "..") == 0) //..�� ��� -> ��� �ϳ� ����
        {
            //���� ������ ���� �̵�
            tmp_delete_ptr = front_dir_list_ptr;
            tmp_delete_back_ptr = NULL;

            while(tmp_delete_ptr->next_ptr != NULL)
            {
                tmp_delete_back_ptr = tmp_delete_ptr;
                tmp_delete_ptr = tmp_delete_ptr->next_ptr;
            }

            //����ó��(��尡 �ϳ� ���� ���)
            if(tmp_delete_back_ptr == NULL)
            {
                printf("�߸��� ����Դϴ�.(root�� �θ� ���丮�� �������� ����.)\n");

                return;
            }

            //������ ��� ����
            free(tmp_delete_ptr->name);
            free(tmp_delete_ptr);
            tmp_delete_back_ptr->next_ptr = NULL;
            rear_dir_list_ptr = tmp_delete_back_ptr;
        }
        else //..�� �ƴ� ��� -> ���� ���丮���� �˻��ؼ� �� ��� �߰�
        {
            //���� ���丮���� �ش� ���ϸ� �˻�, inode ��ȣ ȹ��
            fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20 * (rear_dir_list_ptr->inode - 1), SEEK_SET);
            fread(i_data, sizeof(INODE), 1, myfs);

            dir_file_num = i_data->size / (8 + sizeof(int));
            count = 0;

            fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * i_data->dir_1), SEEK_SET);
            for (int i = 0; i < dir_file_num; i++)
            {
                fread(filename, 8, 1, myfs);
                fread(inodenumber, sizeof(int), 1, myfs);

                if (strcmp(nm_ptr, filename) == 0)
                {
                    break;
                }
                else
                {
                    count++;
                }
            }

            //����ó��(.�� ���)
            if(strcmp(nm_ptr, ".") == 0)
            {
                printf("�߸��� ����Դϴ�.(.�� ���� ���丮�� ��Ÿ��.)\n");

                return;
            }

            if (count == dir_file_num) //���� ���丮�� �ش� ������ ���� ���
            {
                printf("�߸��� ����Դϴ�.(��ο� �������� �ʴ� ���ϸ��� ���Ե�.)\n");

                return;
            }
            else //���� ���丮�� �ش� ������ �ִ� ���
            {
                //���� ���� �˻�
                INODE *file_inode_tmp_ptr = (INODE *)malloc(sizeof(INODE));
                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20 * (*inodenumber - 1), SEEK_SET);
                fread(file_inode_tmp_ptr, sizeof(INODE), 1, myfs);

                if(file_inode_tmp_ptr->type == 1) //�ش� ������ �Ϲ� ������ ���
                {
                    printf("�߸��� ����Դϴ�.(��ο� �Ϲ� ���ϸ��� ���Ե�.)\n");

                    return;
                }
                else //�ش� ������ ���丮�� ���
                {
                    //���ϸ�, inode ��ȣ�� ����ؼ� �� ��� ����
                    rear_dir_list_ptr->next_ptr = (DIR_LIST *)malloc(sizeof(DIR_LIST));
                    rear_dir_list_ptr = rear_dir_list_ptr->next_ptr;
                    rear_dir_list_ptr->inode = *inodenumber;
                    rear_dir_list_ptr->name = (char *)malloc(sizeof(char) * 8);
                    strcpy(rear_dir_list_ptr->name, nm_ptr);
                    rear_dir_list_ptr->next_ptr = NULL;
                }

                free(file_inode_tmp_ptr);
            }
        }
    }
    while((nm_ptr = strtok(NULL, "/")) != NULL);

    //�޸� �ݳ�
    free(i_data);
    free(filename);
    free(inodenumber);

    fclose(myfs);

    return;
}

/*
�̸�    : mycpto �Լ�
�ۼ���  : ���Ժ�
���    : MY ���Ͻý��ۿ� �ִ� ������ ȣ��Ʈ ��ǻ�ͷ� �����Ѵ�.
�޴°�  : ���ϸ� 1,2
���ϰ�  : ���ϰ�
*/
void mycpto (const char* source_file, const char* dest_file  )
{
    //���� ����
    FILE *ofp;
    FILE *myfs;
    int c;
    char d;
    int test = 0;
    int i = 0;
    int inode = 1;
    int tmp_datablock;

    int presentinode = rear_dir_list_ptr-> inode; //���� ���͸��� ���̳�� ��ȣ�� ������ ����
    INODE *presenti_data = (INODE *)malloc(sizeof(INODE)); //���� ���͸��� inode ������ ������ ����ü�� ����ų ������
    INODE *file_inode_tmp_ptr = (INODE *)malloc(sizeof(INODE));//�޾ƿ� ������ inode ������ ������ ����ü�� ����ų ������

    //���� ����ó��
    if(source_file ==NULL || dest_file == NULL)
    {
        printf("���� : ���ڰ� �����մϴ�\n");
        return;
    }

    //���� ����
    myfs = fopen("myfs", "rb");

    //���� ���͸��� ���̳�� ���� �ޱ�
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20*(presentinode - 1), SEEK_SET);
    fread(presenti_data, sizeof(INODE), 1, myfs);

    //�Լ������� ���ϸ�� �����ͺ���� ���ϸ� ��
    int n = presenti_data->size / (8 + sizeof(int)); //for���� ���� ��������
    char *filename = (char *)malloc(sizeof(char) * 8); //���ϸ��� �б����� ����
    int *fileinode = (int *)malloc(sizeof(int));

    unsigned count;
    int none_tmp = 0;

    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data-> dir_1)), SEEK_SET); //���� ���丮�� datablock���� �̵�

    for(int i=0; i<n; i++)
    {
        fread(filename, 8, 1, myfs);
        fread(fileinode, sizeof(int), 1, myfs);

        if(strcmp(source_file, filename) == 0) //���� ��Ͽ� ���� �̸��� ������ �ִ� ���
        {
            count = i;
            break;
        }
        else
        {
            none_tmp++;
        }
    }

    if (none_tmp == n) //�ش� �̸��� ������ �������� �ʴ� ���
    {
        printf("�ش� �̸��� ������ �������� �ʽ��ϴ�.\n");

        return;
    }
    else //�ش� �̸��� ������ �����ϴ� ���
    {
        //�ش� ������ �Ϲ� �������� �˻�
        fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + sizeof(INODE) * (*fileinode - 1), SEEK_SET);
        fread(file_inode_tmp_ptr, sizeof(INODE), 1, myfs);

        if (file_inode_tmp_ptr->type == 0) //�ش� ������ ���͸��� ���
        {
            printf("�ش� ������ �Ϲ� ������ �ƴմϴ�.\n");
            free(presenti_data);
            free(filename);
            return;
        }
    }

    //ȣ��Ʈ ��ǻ���� ���� ����
    if ((ofp = fopen(dest_file, "w")) == NULL)
    {
        printf("���� : ������ ���� ���߽��ϴ�.\n");
        return;
    }

    //datablock �о ȣ��Ʈ ��ǻ�� ���Ͽ� �ۼ�
    int datablock_num; //datablock ��ȣ�� ������ ����(����� ���� + 1 ����)

    for(int print_loop = 0; print_loop < 8; print_loop++)
    {
        if((file_inode_tmp_ptr->size) > (256 * print_loop))
        {
            datablock_num = *((char *)file_inode_tmp_ptr + 11 + print_loop);

            fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + DATA_BLOCK_SIZE * datablock_num, SEEK_SET);

            for(int print_loop_2 = 0; print_loop_2 < 256; print_loop_2++)
            {
                fread(&d, sizeof(char), 1, myfs);

                if(d == -1)
                {
                    break;
                }
                else
                {
                    putc(d, ofp);
                }
            }
        }
    }

    //single indirect�� ����ϴ� ���
    char *indir_dir_num = (char *)malloc(sizeof(char)); //indir�� ���� datablock ��ȣ�� ������ ������ ����ų ������

    if(file_inode_tmp_ptr->size > (256 * 8)) //direct������ ������ �� �ִ� ũ�⺸�� ũ�Ⱑ ū ���
    {
        for(int print_loop = 0; print_loop < 8; print_loop++)
        {
            if((file_inode_tmp_ptr->size - (256 * 8)) > (256 * print_loop))
            {
                //indir�� datablock���� �̵��ؼ� datablock ��ȣ �˻�
                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * file_inode_tmp_ptr->indir) + print_loop, SEEK_SET);
                fread(indir_dir_num, sizeof(char), 1, myfs);
                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + DATA_BLOCK_SIZE * (*indir_dir_num), SEEK_SET);

                for(int print_loop_2 = 0; print_loop_2 < 256; print_loop_2++)
                {
                    fread(&d, sizeof(char), 1, myfs);

                    if(d == -1)
                    {
                        break;
                    }
                    else
                    {
                        putc(d, ofp);
                    }
                }
            }
        }
    }

    free(file_inode_tmp_ptr);
    free(fileinode);
    free(indir_dir_num);

    fclose(ofp);
    fclose(myfs);

    return;
}

/*
�̸�    : mycpfrom �Լ�
�ۼ���  : ���Ժ�
���    : ȣ��Ʈ ��ǻ�Ϳ� �ִ� ������ MY ���Ͻý������� �����Ѵ�.
�޴°�  : ���ϸ� 1,2
���ϰ�  : ���ϰ�
*/
void mycpfrom (char* source_file, char* dest_file )
{
    //���� ����
    FILE *ifp;
    FILE *myfs;
    int c, size_F = 0;
    char d;
    time_t Time;
    struct tm* TimeInfo;

    int presentinode = rear_dir_list_ptr-> inode; //���� ���͸��� ���̳���ȣ
    INODE *presenti_data = (INODE *)malloc(sizeof(INODE)); //���� ���͸��� ���̳�� ����ü
    INODE *file_inode_tmp_ptr = (INODE *)malloc(sizeof(INODE));//�޾ƿ� ������ ���̳�� ����ü

    //���� ����ó��
    if(source_file == NULL || dest_file == NULL)
    {
        printf("���� : ���ڰ� �����մϴ�\n");

        free(presenti_data);
        free(file_inode_tmp_ptr);

        return;
    }

    //ȣ��Ʈ ��ǻ���� ���� ����
    if ((ifp = fopen(source_file, "r")) == NULL)
    {
        printf("���� : %s ������ �������� �ʰų� �� �� �����ϴ�.\n", source_file);
        
        free(presenti_data);
        free(file_inode_tmp_ptr);

        return;
    }

    //���� ����
    myfs = fopen("myfs", "rb+");

    //���� ���͸��� ���̳�� �ޱ�
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20 * (presentinode - 1), SEEK_SET);
    fread(presenti_data, sizeof(INODE), 1, myfs);
    
    //�Լ������� ���ϸ�� �����ͺ���� ���ϸ� ��
    int n = presenti_data->size / (8 + sizeof(int)); //for���� ���� ��������
    char *filename = (char *)malloc(sizeof(char) * 8); //���ϸ��� �б����� ����
    int *fileinode = (int *)malloc(sizeof(int));

    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data->dir_1)), SEEK_SET);
    for(int i = 0; i < n; i++)
    {
        fread(filename, 8, 1, myfs);
        fread(fileinode, sizeof(int), 1, myfs);

        if(strcmp(dest_file, filename) == 0)
        {
            printf("�ش� �̸��� ������ �����մϴ�.\n");

            free(presenti_data);
            free(file_inode_tmp_ptr);
            free(filename);
            free(fileinode);

            return;
        }
    }
    
    //���� �ݱ�
    fclose(myfs);

    //dest_file�� �̸��� ������ ���� ����
    mytouch(dest_file);

    //���� ����
    myfs = fopen("myfs", "rb+");

    //�ش� ������ inode ���� ���
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20 * (presentinode - 1), SEEK_SET);
    fread(presenti_data, sizeof(INODE), 1, myfs);
    n = presenti_data->size / (8 + sizeof(int));

    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data->dir_1)), SEEK_SET);    
    for(int i = 0; i < n; i++)
    {
        fread(filename, 8, 1, myfs);
        fread(fileinode, sizeof(int), 1, myfs);

        if(strcmp(dest_file, filename) == 0)
        {
            break;
        }
    }

    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + sizeof(INODE) * (*fileinode - 1), SEEK_SET);
    fread(file_inode_tmp_ptr, sizeof(INODE), 1, myfs);

    //ȣ��Ʈ ��ǻ���� ���� ũ�� �˾Ƴ���.
    int host_file_size;
    fseek(ifp, 0, SEEK_END);
    host_file_size = ftell(ifp);
    rewind(ifp);

    //ȣ��Ʈ ��ǻ�� ���� �о datablock�� �ۼ�
    char minousone = -1; //���� ���� ������ -1�� ������ ����
    int savedbnumber = 0; //datablock ��ȣ�� ������ ����
    SUPERBLOCK *sb_data = (SUPERBLOCK *)malloc(sizeof(SUPERBLOCK));

    for(int print_loop = 0; print_loop < 8; print_loop++)
    {
        if(host_file_size > (256 * print_loop))
        {
            //datablock �Ҵ��ϱ�
            if(print_loop == 0) //�� ó�� ������ ���->�̹� datablock �ϳ��� �Ҵ�Ǿ� ����
            {
                savedbnumber = file_inode_tmp_ptr->dir_1 + 1;
            }
            else //ó�� ������ �ƴ� ���->���ο� datablock �Ҵ� �ʿ�
            {
                //�� datablock �Ҵ��ϱ�
                fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
                fread(sb_data, sizeof(SUPERBLOCK), 1, myfs);
                
                savedbnumber = 0;
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_1);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_2);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_3);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_4);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_5);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_6);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_7);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_8);
                savedbnumber++;

                //datablock�� �ݿ��ϱ�
                change_superblock(0, savedbnumber, sb_data);

                fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
                fwrite(sb_data, sizeof(SUPERBLOCK), 1, myfs);

                //inode�� datablock ��ȣ �ݿ��ϱ�
                *((char *)file_inode_tmp_ptr + 11 + print_loop) = (unsigned)(savedbnumber - 1);
            }

            //datablock�� ���� �ۼ��ϱ�
            for(int print_loop_2 = 0; print_loop_2 < 256; print_loop_2++)
            {
                d = getc(ifp);

                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (savedbnumber - 1)) + print_loop_2, SEEK_SET);

                if(d == EOF)
                {
                    fwrite(&minousone, sizeof(char), 1, myfs);

                    break;
                }
                else
                {
                    fwrite(&d, sizeof(char), 1, myfs);
                }
            }
        }
    }

    //single indirect�� ����ϴ� ���
    char *indir_dir_num = (char *)malloc(sizeof(char)); //indir�� ���� datablock ��ȣ�� ������ ������ ����ų ������
    unsigned char indir_db_tmp; //indir�� ������ datablock ��ȣ�� �ӽ� ������ ����

    if(host_file_size > (256 * 8)) //direct������ ������ �� �ִ� ũ�⺸�� ũ�Ⱑ ū ���
    {
        //indir datablock �Ҵ��ϱ�
        fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
        fread(sb_data, sizeof(SUPERBLOCK), 1, myfs);
        
        savedbnumber = 0;
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_1);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_2);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_3);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_4);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_5);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_6);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_7);
        savedbnumber = checkbit(savedbnumber, sb_data->data_block_8);
        savedbnumber++;

        change_superblock(0, savedbnumber, sb_data);

        fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
        fwrite(sb_data, sizeof(SUPERBLOCK), 1, myfs);

        //inode�� datablock ��ȣ �ݿ��ϱ�
        file_inode_tmp_ptr->indir = (unsigned)(savedbnumber - 1);

        //indir datablock �⺻�� �ۼ��ϱ�
        fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + DATA_BLOCK_SIZE * (savedbnumber - 1), SEEK_SET);
        char zero_db_value = 0;
        for(int loop_zero = 0; loop_zero < 8; loop_zero++)
        {
            fwrite(&zero_db_value, sizeof(char), 1, myfs);
        }
        fwrite(&minousone, sizeof(char), 1, myfs);

        //���� ũ�� �˻��ؼ� datablock�� �ۼ��ϱ�
        for(int print_loop = 0; print_loop < 8; print_loop++)
        {
            if((host_file_size - (256 * 8)) > (256 * print_loop))
            {
                //indir�� datablock�� �ۼ��� datablock �Ҵ��ϱ�
                fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
                fread(sb_data, sizeof(SUPERBLOCK), 1, myfs);
                
                savedbnumber = 0;
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_1);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_2);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_3);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_4);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_5);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_6);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_7);
                savedbnumber = checkbit(savedbnumber, sb_data->data_block_8);
                savedbnumber++;

                //datablock�� �ݿ��ϱ�
                change_superblock(0, savedbnumber, sb_data);

                fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
                fwrite(sb_data, sizeof(SUPERBLOCK), 1, myfs);

                //indir�� datablock�� ���� �Ҵ��� datablock ��ȣ �ݿ��ϱ�
                indir_db_tmp = savedbnumber - 1;
                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + DATA_BLOCK_SIZE * (file_inode_tmp_ptr->indir) + print_loop, SEEK_SET);
                fwrite(&indir_db_tmp, sizeof(char), 1, myfs);

                //�Ҵ��� datablock�� ���� �ۼ��ϱ�
                fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + DATA_BLOCK_SIZE * (savedbnumber - 1), SEEK_SET);
                for(int print_loop_2 = 0; print_loop_2 < 256; print_loop_2++)
                {
                    d = getc(ifp);

                    if(d == EOF)
                    {
                        fwrite(&minousone, sizeof(char), 1, myfs);

                        break;
                    }
                    else
                    {
                        fwrite(&d, sizeof(char), 1, myfs);
                    }
                }
            }
        }
    }

    //inode�� size �����ϱ�
    file_inode_tmp_ptr -> size = host_file_size;
    
    //inode �ݿ��ϱ�
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + sizeof(INODE) * (*fileinode - 1), SEEK_SET);
    fwrite(file_inode_tmp_ptr, sizeof(INODE), 1, myfs);

    free(presenti_data);
    free(file_inode_tmp_ptr);
    free(filename);
    free(fileinode);
    free(sb_data);
    free(indir_dir_num);

    fclose(ifp);
    fclose(myfs);

    return;
}

/*
�̸�    : mycp �Լ�
�ۼ���  : ���Ժ�
���    : MY ���Ͻý��ۿ� �ִ� ������ �����ϴ� ��ɾ�
�޴°�  : ���ϸ� 1,2
���ϰ�  : ���ϰ�
*/

void mycp(char* source_file, char* dest_file  )
{
    FILE*fp;

    if(source_file ==NULL || dest_file == NULL)
    {
        printf("���� : ���ڰ� �����մϴ�");
        return;
    }//���ڰ� ������ ��� ����ó��

    if(strcmp(source_file, dest_file) == 0)
    {
        printf("���� : ���ڰ� �����ϴ�");
        return;
    }//���ڰ� ������� ����ó��

    fp = fopen("tmp","wb");
    fclose(fp);
    
    mycpto(source_file,"tmp");
    mycpfrom("tmp",dest_file);
    
    system("rm tmp");

    return;
}

/*
�̸�    : myrm �Լ�
�ۼ���  : ���μ�
���    : ������ �����ϴ� ��ɾ�
�޴°�  : ���ϸ�
���ϰ�  : x
*/

void myrm(const char* file)
{
    FILE *myfs;
    myfs = fopen("myfs", "rb+");

    //���� ���͸��� ���̳�� �ޱ�
    int presentinode = rear_dir_list_ptr-> inode; //���� ���͸��� ���̳���ȣ
    INODE *presenti_data = (INODE *)malloc(sizeof(INODE)); //���� ���͸��� ���̳�� ����ü
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20*(presentinode-1), SEEK_SET);
    fread(presenti_data, sizeof(INODE), 1, myfs);
    
    //�Լ������� ���ϸ�� �����ͺ���� ���ϸ� ��
    int n = presenti_data-> size/12; //for���� ���� ��������
    char *filename = (char *)malloc(sizeof(char) * 8); //���ϸ��� �б����� ����
    int *fileinode = (int *)malloc(sizeof(int));
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data-> dir_1)), SEEK_SET);
    unsigned count;
    int none_tmp = 0;
    for(int i=0; i<n; i++)
    {
        fread(filename, 8, 1, myfs);
        fread(fileinode, sizeof(int), 1, myfs);
        if(strcmp(file, filename) == 0)
        {
            count = i;
            break;
        }
        else
        {
            none_tmp++;
        }
    }

    if (none_tmp == n) //�ش� �̸��� ������ �������� �ʴ� ���
    {
        printf("�ش� �̸��� ������ �������� �ʽ��ϴ�.\n");

        return;
    }
    else //�ش� �̸��� ������ �����ϴ� ���
    {
        //�ش� ������ �Ϲ� �������� �˻�
        INODE *file_inode_tmp_ptr = (INODE *)malloc(sizeof(INODE));
        fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + sizeof(INODE) * (*fileinode - 1), SEEK_SET);
        fread(file_inode_tmp_ptr, sizeof(INODE), 1, myfs);
        if (file_inode_tmp_ptr->type == 0) //�ش� ������ ���͸��� ���
        {
            printf("�ش� ������ �Ϲ� ������ �ƴմϴ�.\n");

            free(file_inode_tmp_ptr);
            free(fileinode);
            free(presenti_data);
            free(filename);

            return;
        }
        else //�ش� ������ �Ϲ� ������ ���
        {
            free(file_inode_tmp_ptr);
        }
    }

    //������ ������ ���̳�� ȣ��
    INODE *i_data = (INODE *)malloc(sizeof(INODE)); //������ ������ ���̳�� ����ü
    fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20*(*fileinode-1), SEEK_SET);
    fread(i_data, sizeof(INODE), 1, myfs);
    int saveinumber = *fileinode; //������ ������ ���̳�� ��ȣ����
    if(none_tmp < n && i_data-> type == 1)
    {
        //���� ����
        for(int i = count; i<(n); i++)
        {
            fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data->dir_1)) + (8 + sizeof(int)) * (i + 1), SEEK_SET);
            fread(filename, 8, 1, myfs);
            fread(fileinode, sizeof(int), 1, myfs);
            fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data->dir_1)) + (8 + sizeof(int)) * (i), SEEK_SET);
            fwrite(filename, 8, 1, myfs);
            fwrite(fileinode, sizeof(int), 1, myfs);            
        }
        fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + INODE_LIST_SIZE + (DATA_BLOCK_SIZE * (presenti_data->dir_1)) + (8 + sizeof(int)) * (n - 1), SEEK_SET);
        char *minusone = (char *)malloc(sizeof(char));
        *minusone = -1;
        fwrite(minusone, sizeof(char), 1, myfs);
        free(minusone);

        //���ۺ�� ����
        SUPERBLOCK *sb_data = (SUPERBLOCK *)malloc(sizeof(SUPERBLOCK)); 
        fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
        fread(sb_data, sizeof(SUPERBLOCK), 1, myfs);
        unsigned mask = 1 << 31;
        if(saveinumber>0 && saveinumber<=32)
        {
            mask >>= (saveinumber - 1);
            sb_data-> inode_1 = sb_data-> inode_1 ^ mask;
        }
        else if(saveinumber>33 && saveinumber<=64)
        {
            mask >>= (saveinumber - 33);
            sb_data-> inode_2 = sb_data-> inode_2 ^ mask;
        }
        else if(saveinumber>65 && saveinumber<=96)
        {
            mask >>= (saveinumber - 65);
            sb_data-> inode_3 = sb_data-> inode_3 ^ mask;
        }
        else if(saveinumber>97 && saveinumber<=128)
        {
            mask >>= (saveinumber - 97);
            sb_data-> inode_4 = sb_data-> inode_4 ^ mask;
        }
        unsigned mask1 = 1 << 31;
        if(i_data-> dir_1 >= 0 && i_data-> dir_1 <32)
        {
            mask1 >>= (i_data-> dir_1);
            sb_data-> data_block_1 = sb_data-> data_block_1 ^ mask1;
        }
        else if(i_data-> dir_1 >= 32 && i_data-> dir_1 <64)
        {
            mask1 >>= (i_data-> dir_1 - 32);
            sb_data-> data_block_2 = sb_data-> data_block_2 ^ mask1;
        }
        else if(i_data-> dir_1 >= 64 && i_data-> dir_1 <96)
        {
            mask1 >>= (i_data-> dir_1 - 64);
            sb_data-> data_block_3 = sb_data-> data_block_3 ^ mask1;
        }
        else if(i_data-> dir_1 >= 96 && i_data-> dir_1 <128)
        {
            mask1 >>= (i_data-> dir_1 - 96);
            sb_data-> data_block_4 = sb_data-> data_block_4 ^ mask1;
        }
        else if(i_data-> dir_1 >= 128 && i_data-> dir_1 <160)
        {
            mask1 >>= (i_data-> dir_1 - 128);
            sb_data-> data_block_5 = sb_data-> data_block_5 ^ mask1;
        }
        else if(i_data-> dir_1 >= 160 && i_data-> dir_1 <192)
        {
            mask1 >>= (i_data-> dir_1 - 160);
            sb_data-> data_block_6 = sb_data-> data_block_6 ^ mask1;
        }
        else if(i_data-> dir_1 >= 192 && i_data-> dir_1 <224)
        {
            mask1 >>= (i_data-> dir_1 - 192);
            sb_data-> data_block_7 = sb_data-> data_block_7 ^ mask1;
        }
        else if(i_data-> dir_1 >= 224 && i_data-> dir_1 <256)
        {
            mask1 >>= (i_data-> dir_1 - 224);
            sb_data-> data_block_8 = sb_data-> data_block_8 ^ mask1;
        }
        fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
        fwrite(sb_data, sizeof(SUPERBLOCK), 1, myfs);
        free(sb_data);

        //������͸� ������ ����
        presenti_data-> size -= (8 + sizeof(int));
        fseek(myfs, BOOT_BLOCK_SIZE + SUPER_BLOCK_SIZE + 20*(presentinode-1), SEEK_SET);
        fwrite(presenti_data, sizeof(INODE), 1 ,myfs);
    }

    free(presenti_data);
    free(filename);
    free(fileinode);
    free(i_data);
    fclose(myfs);
    return;
}
/*
�̸�    : cntfound �Լ�
�ۼ���  : ���Ժ�
���    : dir_list���� ���� �̾��� ���丮 �� ����
�޴°�  : X
���ϰ�  : count��
*/

int cntfound() 
{
    int cnt = 0;
    DIR_LIST* tmp_dir = front_dir_list_ptr;
    while ((tmp_dir->next_ptr) != NULL)
    {
        {
            tmp_dir = tmp_dir->next_ptr;
            cnt++;
            printf("fff\n");
        }
    }
    return cnt;//Ž�� ���н�

}

/*
�̸�    : prtpwd �Լ�
�ۼ���  : ���Ժ�
���    : pwd�� ���ڿ��� ����
�޴°�  : X
���ϰ�  : pwd ���ڿ���
*/
char* prtpwd()
{
    char* s;
    char* pwd;
    DIR_LIST *tmp_ptr = front_dir_list_ptr;

    while (1)
    {
        s = tmp_ptr->name;
        pwd = strcat(pwd,s);
        if (tmp_ptr->next_ptr == NULL)
        {
            break;
        }
        else
        {
            pwd = strcat(pwd,"/");
            tmp_ptr = tmp_ptr->next_ptr;
        }
    }
    return pwd;
}

/*
�̸�    : acc_inode �Լ�
�ۼ���  : ���Ժ�
���    : ���������� ���� inode ��ȣ�� ��������, �� inode��ȣ�� �ش��ϴ� ���ۺ���� 1�� ä���.
�޴°�  : X
���ϰ�  : ���������� ���� inode ��ȣ
*/

int acc_inode()
{
    FILE* myfs;
    int t = 1;

    myfs = fopen("myfs", "rb");
    if (myfs == NULL)
    {
        printf("acc_inode() �Լ� : ���� ���⿡ �����߽��ϴ�.\n");
        abort();
    }

    SUPERBLOCK *sb_ptr = (SUPERBLOCK *)malloc(sizeof(SUPERBLOCK));
    fseek(myfs, BOOT_BLOCK_SIZE, SEEK_SET);
    fread(sb_ptr, sizeof(SUPERBLOCK), 1, myfs);//������� ���ۺ�� Ȯ���Ͽ� ���������� ���� ���̳�忡 ���� �ֱ� ����.

    unsigned mask;
    while(1){ // �� inode�� ������ ���� ���� �ݺ�
    if (t < 33) // inode_1�� �� inode�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->inode_1 & mask) == 0) //�� ���̳�尡 �־��� ���
        {
            sb_ptr->inode_1 = sb_ptr->inode_1 | 1;//�� ���̳�� ä���
            break;
        }
        t++;
    }
    else if (t < 65) // inode_2�� �� inode�� ���� ���
    {
        mask = 1 << ((t- 32) - 1);
        if ((sb_ptr->inode_2 & mask) == 0)
        {
            sb_ptr->inode_2 = sb_ptr->inode_2 | 1;
            break;
        }
        t++;
    }
    else if (t < 97) // inode_3�� �� inode�� ���� ���
    {
        mask = 1 << ((t - 64) - 1);
        if ((sb_ptr->inode_3 & mask) == 0)
        {
          sb_ptr->inode_3 = sb_ptr->inode_3 | 1;
          break;
        }
        t++;
    }
    else // inode_4�� �� inode�� ���� ���
    {
        mask = 1 << ((t - 96) - 1);
        if ((sb_ptr->inode_4 & mask) == 0)
        {
            sb_ptr->inode_4 = sb_ptr->inode_4 | 1;
            break;
        }
        t++;
    }
    }
    return t;
}

/*
�̸�    : acc_data �Լ�
�ۼ���  : ���Ժ�
���    : ���������� ���� datablock ��ȣ�� ��������, �� inode��ȣ�� �ش��ϴ� ���ۺ���� 1�� ä���.
�޴°�  : X
���ϰ�  : ���������� ���� datablock ��ȣ
*/
int acc_data()
{
    FILE* myfs;
    int t = 1;

    myfs = fopen("myfs", "rb");
    if (myfs == NULL)
    {
        printf("acc_data() �Լ� : ���� ���⿡ �����߽��ϴ�.\n");
        abort();
    }

    SUPERBLOCK *sb_ptr = (SUPERBLOCK *)malloc(sizeof(SUPERBLOCK));
    fseek(myfs, BOOT_BLOCK_SIZE+128, SEEK_SET);
    fread(sb_ptr, sizeof(SUPERBLOCK), 1, myfs);//������� ���ۺ�� Ȯ���Ͽ� ���������� ���� �����ͺ�Ͽ� ���� �ֱ� ����.

    unsigned mask;
    while(1)
    { // �� �����ͺ���� ������ ���� ���� �ݺ�
    if (t < 33) // data_block_1�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_1 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_1 = sb_ptr->data_block_1 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 65) // data_block_2�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_2 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_2 = sb_ptr->data_block_2 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 97) // data_block_3�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_3 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_3 = sb_ptr->data_block_3 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 129) // data_block_4�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_4 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_4 = sb_ptr->data_block_4 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 161) // data_block_5�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_5 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_5 = sb_ptr->data_block_5 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 193) // data_block_6�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_6 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_6 = sb_ptr->data_block_6 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else if (t < 225) // data_block_7�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_7 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_7 = sb_ptr->data_block_7 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    else // data_block_3�� �� data_block�� ���� ���
    {
        mask = 1 << (t - 1);
        if ((sb_ptr->data_block_8 & mask) == 0) //�� ���ۺ���� �־��� ���
        {
            sb_ptr->data_block_8 = sb_ptr->data_block_8 | 1;//�� ���ۺ�� ä���
            break;
        }
        t++;
    }
    }
    return t;
}
