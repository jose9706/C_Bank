#include <stdio.h>
#include "bank.h"
#include <stdlib.h>
#include "account.h"
#include <string.h>
#include <time.h>
#include <errno.h>

#define GENERIC_BUFFER_SIZE 100

void remove_newlineOrSpacesAtTheEnd(char *str) // From internet, added space deletion manually
{
    // Find the newline character in the string
    size_t len = strcspn(str, "\n");
    if (str[len - 1] == ' ')
    {
        while (str[len - 1] == ' ')
        {
            len--;
        }
        str[len] = '\0';
    }
    else
    {
        if (str[len] == '\n')
        {
            str[len] = '\0'; // Replace newline with null terminator
        }
    }
}

int is_float(const char *str) // From internet
{
    char *endptr;
    errno = 0;
    double temp = strtod(str, &endptr);

    // Check for various errors
    if (errno != 0)
    {
        return 0; // Not a valid floating-point number
    }

    if (endptr == str || *endptr != '\0')
    {
        return 0; // No digits were found or extra characters after the number
    }

    return 1; // It's a valid floating-point number
}

void FreeAccs(struct account *accs, const int count)
{
    for (size_t i = 0; i < count; i++)
    {
        free(accs[i].name);
    }

    free(accs);
}

void DisplayMenu(const int accountCount)
{
    printf("Welcome to the bank simulator in C.\n");
    printf("Current accounts in the bank: %d\n", accountCount);
    printf("Select option:\n");
    printf("1. Add new account.\n");
    printf("2. Check balance of a given account.\n");
    printf("3. Withdraw money of account.\n");
    printf("4. Deposit money into account.\n");
    printf("5. Exit.\n");
}

void CreateAccount(struct account **accountList, int *count)
{
    *accountList = (struct account *)realloc(*accountList, (*count + 1) * sizeof(struct account));
    if (*accountList == NULL)
    {
        printf("Failed allocating memory for new account...\n");
        return;
    }

    // Access the new account slot
    struct account *newAccount = &(*accountList)[*count];

    char name[GENERIC_BUFFER_SIZE];
    printf("Input account name:");
    if (fgets(name, sizeof(name), stdin) == NULL)
    {
        printf("Failed getting input name...");
        return;
    }
    remove_newlineOrSpacesAtTheEnd(name);

    printf("Input initial deposit:");
    char initialDeposit[GENERIC_BUFFER_SIZE];
    if (fgets(initialDeposit, sizeof(initialDeposit), stdin) == NULL)
    {
        printf("Failed getting input initialDeposit...");
        return;
    }
    remove_newlineOrSpacesAtTheEnd(initialDeposit);

    if (is_float(initialDeposit))
    {
        newAccount->balance = atof(initialDeposit);
    }
    else
    {
        printf("Invalid balanced input. Restart creation step.\n");
        return;
    }
    newAccount->name = (char *)calloc(GENERIC_BUFFER_SIZE, sizeof(char));
    if (newAccount->name == NULL)
    {
        printf("Failed allocating space for the account name.\n");
        return;
    }

    strcpy(newAccount->name, name);
    (*count)++;
    srand((unsigned int)time(NULL));
    newAccount->id = rand();
}

size_t FindAccountLoc(const struct account *accs, const int count, const char *nameToFind)
{
    size_t i;
    for (i = 0; i < count; i++)
    {
        if (strcmp(accs[i].name, nameToFind) == 0)
        {
            return i;
        }
    }

    return count + 999; // Random number just do its so wrong we assume its not found.
}

// 1 Mode = deposit
// 2 mode = withdraw
void HandleCash(struct account *accs, const int count, const char *nameToFind, const int mode)
{
    size_t loc = FindAccountLoc(accs, count, nameToFind);
    if (loc > count)
    {
        printf("Couldn't find given name to find. Skipping\n");
        return;
    }

inputForDeposit:
    printf("Input money to deposit:");
    char buff[GENERIC_BUFFER_SIZE];
    if (fgets(buff, sizeof(buff), stdin) == NULL)
    {
        printf("Failed getting value. Retry");
        goto inputForDeposit;
    }

    remove_newlineOrSpacesAtTheEnd(buff);

    if (is_float(buff))
    {
        double inputBalance = atof(buff);

        if (mode == 1)
        {
            if (inputBalance < 0)
            {
                printf("Can't deposit negative balance. Retry\n");
                goto inputForDeposit;
            }

            accs[loc].balance += inputBalance;
            printf("Account new balance is:%0.1f\n", accs[loc].balance);
            return;
        }
        else
        {
            if (mode == 0)
            {
                if (inputBalance > accs[loc].balance)
                {
                    printf("Can't withdraw more than %f, try a value lower than that.\n", accs[loc].balance);
                    goto inputForDeposit;
                }

                accs[loc].balance -= inputBalance;
                printf("Account new balance is:%0.1f\n", accs[loc].balance);
                return;
            }
            else
            {
                printf("how did we even get here, FAIL");
                return;
            }
        }
    }
    else
    {
        printf("Input value %s is not a double.", buff);
        goto inputForDeposit;
    }
}

void DisplayBalanceOfAccount(const struct account *accs, const int count, const char *nameToFind)
{
    size_t loc = FindAccountLoc(accs, count, nameToFind);
    if (loc > count)
    {
        printf("Couldn't find given name to find. Skipping\n");
        return;
    }
    printf("Balance of=[%s] with id=[%d] and balance=[%1.f]\n", accs[loc].name, accs[loc].id, accs[loc].balance);
}

char *AccountToDisplay()
{
    printf("Input account to find:");
    char *nameToFindTemp = (char *)calloc(GENERIC_BUFFER_SIZE, sizeof(char));
    if (fgets(nameToFindTemp, GENERIC_BUFFER_SIZE, stdin) == NULL)
    {
        printf("Failed getting input name to find...");
    }
    remove_newlineOrSpacesAtTheEnd(nameToFindTemp);
    return nameToFindTemp;
}

void RunBank()
{
    int accountCount = 0;
    struct account *accounts = NULL; // always NULL
    DisplayMenu(accountCount);
    while (1)
    {
        char option[GENERIC_BUFFER_SIZE];
        if (fgets(option, sizeof(option), stdin) == NULL)
        {
            printf("Failed getting input option...");
        }
        int inputOption = atoi(option);
        switch (inputOption)
        {
        case 1:
            CreateAccount(&accounts, &accountCount);
            break;
        case 2:
            char *nameToDisplay = AccountToDisplay();
            DisplayBalanceOfAccount(accounts, accountCount, nameToDisplay);
            free(nameToDisplay);
            break;
        case 3:
            char *nameToFind = AccountToDisplay();
            HandleCash(accounts, accountCount, nameToFind, 0);
            free(nameToFind);
            break;
        case 4:
            char *nameToProcess = AccountToDisplay();
            HandleCash(accounts, accountCount, nameToProcess, 1);
            free(nameToProcess);
            break;
        case 5:
            FreeAccs(accounts, accountCount);
            return;
        default:
            printf("Option %d isnt valid.", inputOption);
            break;
        }

        printf("Continue? [Y/N]\n");
        if (fgets(option, sizeof(option), stdin) == NULL)
        {
            printf("Failed getting input option...");
        }

        if (*option == 'N' || *option == 'n')
        {
            printf("Closing...");
            FreeAccs(accounts, accountCount);
            return;
        }
        else
        {
            printf("Choose next action:");
        }
    }
}