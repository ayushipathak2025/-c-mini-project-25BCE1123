#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define MAX_NAME_LEN 60
#define MAX_REG_NO_LEN 20
#define MAX_SUBJECT_LEN 50
#define MAX_SUBJECTS 10
#define DATA_FILE "students.txt"
#define SUBJECTS_FILE "subjects.txt"

int numSubjects = 3;
char subjectNames[MAX_SUBJECTS][MAX_SUBJECT_LEN] = {"Subject 1", "Subject 2", "Subject 3"};

typedef struct
{
    char regNo[MAX_REG_NO_LEN];
    char name[MAX_NAME_LEN];
    float marks[MAX_SUBJECTS];
    float total;
    float average;
    char grade;
} Student;

void loadSubjects(void)
{
    FILE *fp = fopen(SUBJECTS_FILE, "r");
    if (!fp)
        return;
    if (fscanf(fp, "%d\n", &numSubjects) != 1)
    {
        numSubjects = 3;
        fclose(fp);
        return;
    }
    if (numSubjects < 1 || numSubjects > MAX_SUBJECTS)
        numSubjects = 3;
    for (int i = 0; i < numSubjects; i++)
    {
        if (fgets(subjectNames[i], MAX_SUBJECT_LEN, fp) == NULL)
            break;
        size_t len = strlen(subjectNames[i]);
        if (len > 0 && subjectNames[i][len - 1] == '\n')
            subjectNames[i][len - 1] = '\0';
    }
    fclose(fp);
}

void saveSubjects(void)
{
    FILE *fp = fopen(SUBJECTS_FILE, "w");
    if (!fp)
        return;
    fprintf(fp, "%d\n", numSubjects);
    for (int i = 0; i < numSubjects; i++)
    {
        fprintf(fp, "%s\n", subjectNames[i]);
    }
    fclose(fp);
}

void safeReadLine(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
}

int readInt(const char *prompt)
{
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    getchar(); // consume newline
    return value;
}

void readString(const char *prompt, char *output, int size)
{
    while (1)
    {
        printf("%s", prompt);
        safeReadLine(output, size);
        if (output[0] == '\0')
        {
            printf("Input cannot be empty. Try again.\n");
            continue;
        }
        if (strchr(output, '|') != NULL)
        {
            printf("Input cannot contain '|'. Try again.\n");
            continue;
        }
        return;
    }
}

float readFloat(const char *prompt, float min, float max)
{
    float value;
    printf("%s", prompt);
    scanf("%f", &value);
    getchar(); // consume newline
    return value;
}

char computeGrade(float avg)
{
    if (avg >= 80.0f)
        return 'A';
    if (avg >= 60.0f)
        return 'B';
    if (avg >= 40.0f)
        return 'C';
    return 'F';
}

int loadStudents(Student *list, int maxCount)
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp)
    {
        return 0;
    }
    int count = 0;
    char line[1024];
    while (count < maxCount && fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, "|");
        if (!token)
            continue;
        strcpy(list[count].regNo, token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        strcpy(list[count].name, token);

        for (int i = 0; i < numSubjects; i++)
        {
            token = strtok(NULL, "|");
            if (!token)
                break;
            list[count].marks[i] = atof(token);
        }

        token = strtok(NULL, "|");
        if (!token)
            continue;
        list[count].total = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        list[count].average = atof(token);

        token = strtok(NULL, "|");
        if (!token)
            continue;
        list[count].grade = token[0];

        count++;
    }
    fclose(fp);
    return count;
}

int saveStudents(const Student *list, int count)
{
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp)
    {
        printf("Error opening file for write.\n");
        return 0;
    }
    for (int i = 0; i < count; i++)
    {
        const Student *s = &list[i];
        fprintf(fp, "%s|%s", s->regNo, s->name);
        for (int j = 0; j < numSubjects; j++)
        {
            fprintf(fp, "|%.2f", s->marks[j]);
        }
        fprintf(fp, "|%.2f|%.2f|%c\n", s->total, s->average, s->grade);
    }
    fclose(fp);
    return 1;
}

void configureSubjects(void)
{
    printf("\n>>> Configure Subjects <<<\n");
    int newNum = readInt("Enter number of subjects (1-10): ");
    if (newNum < 1 || newNum > MAX_SUBJECTS)
    {
        printf("Invalid number. Keeping current configuration.\n");
        return;
    }
    numSubjects = newNum;
    for (int i = 0; i < numSubjects; i++)
    {
        printf("Current name for Subject %d: %s\n", i + 1, subjectNames[i]);
        readString("Enter new name: ", subjectNames[i], MAX_SUBJECT_LEN);
    }
    saveSubjects();
    printf("Subjects configured successfully.\n");
}

void displaySubjectConfig(void)
{
    printf("\n>>> Subject Configuration <<<\n");
    printf("Number of subjects: %d\n", numSubjects);
    for (int i = 0; i < numSubjects; i++)
    {
        printf("Subject %d: %s\n", i + 1, subjectNames[i]);
    }
}

void trimWhitespace(char *str)
{
    if (!str)
        return;

    char *start = str;
    while (*start && isspace((unsigned char)*start))
        start++;

    char *end = start + strlen(start) - 1;
    while (end >= start && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';

    if (start != str)
    {
        memmove(str, start, end - start + 2);
    }
}

int equalsIgnoreCase(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void importFromCSV(void)
{
    printf("\n>>> Import Students from CSV <<<\n");
    char csvPath[260];
    readString("Enter CSV file path: ", csvPath, sizeof(csvPath));

    FILE *csv = fopen(csvPath, "r");
    if (!csv)
    {
        printf("Unable to open CSV file: %s\n", csvPath);
        return;
    }

    char line[1024];
    if (!fgets(line, sizeof(line), csv))
    {
        printf("CSV file is empty.\n");
        fclose(csv);
        return;
    }

    // load current students first using current subject count
    int oldNumSubjects = numSubjects;
    Student students[1000];
    int count = loadStudents(students, 1000);

    // parse header row
    char *header = strdup(line);
    if (!header)
    {
        printf("Memory error.\n");
        fclose(csv);
        return;
    }

    char *tokenList[MAX_SUBJECTS + 2];
    int tokenCount = 0;
    char *token = strtok(header, ",\r\n");
    while (token && tokenCount < MAX_SUBJECTS + 2)
    {
        trimWhitespace(token);
        tokenList[tokenCount++] = token;
        token = strtok(NULL, ",\r\n");
    }

    if (tokenCount < 3)
    {
        printf("CSV header must include regNo,name, and at least one subject.\n");
        free(header);
        fclose(csv);
        return;
    }

    // if header names look like actual headings, skip first row data mode
    if (!equalsIgnoreCase(tokenList[0], "regNo") && !equalsIgnoreCase(tokenList[0], "regno"))
    {
        printf("CSV header should start with regNo or regno.\n");
        free(header);
        fclose(csv);
        return;
    }

    int newSubjectCount = tokenCount - 2;
    if (newSubjectCount < 1 || newSubjectCount > MAX_SUBJECTS)
    {
        printf("Subject count in CSV must be 1-%d.\n", MAX_SUBJECTS);
        free(header);
        fclose(csv);
        return;
    }

    numSubjects = newSubjectCount;
    for (int i = 0; i < numSubjects; i++)
    {
        snprintf(subjectNames[i], MAX_SUBJECT_LEN, "%s", tokenList[i + 2]);
    }
    saveSubjects();

    // normalize existing records to new numSubjects
    for (int i = 0; i < count; i++)
    {
        float total = 0.0f;
        for (int j = 0; j < numSubjects; j++)
        {
            if (j < oldNumSubjects)
                total += students[i].marks[j];
            else
                students[i].marks[j] = 0.0f;
        }
        students[i].total = total;
        students[i].average = total / numSubjects;
        students[i].grade = computeGrade(students[i].average);
    }

    int imported = 0;
    while (fgets(line, sizeof(line), csv))
    {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        char *row = strdup(line);
        if (!row)
            break;

        char *fields[MAX_SUBJECTS + 2];
        int fieldCount = 0;
        char *tok = strtok(row, ",\r\n");
        while (tok && fieldCount < numSubjects + 2)
        {
            trimWhitespace(tok);
            fields[fieldCount++] = tok;
            tok = strtok(NULL, ",\r\n");
        }

        if (fieldCount != numSubjects + 2)
        {
            free(row);
            continue;
        }

        // duplicate check
        int exists = 0;
        for (int k = 0; k < count; k++)
        {
            if (strcmp(students[k].regNo, fields[0]) == 0)
            {
                exists = 1;
                break;
            }
        }
        if (exists)
        {
            free(row);
            continue;
        }

        Student s;
        strncpy(s.regNo, fields[0], MAX_REG_NO_LEN);
        s.regNo[MAX_REG_NO_LEN - 1] = '\0';
        strncpy(s.name, fields[1], MAX_NAME_LEN);
        s.name[MAX_NAME_LEN - 1] = '\0';
        s.total = 0.0f;
        for (int j = 0; j < numSubjects; j++)
        {
            s.marks[j] = (float)atof(fields[j + 2]);
            if (s.marks[j] < 0.0f)
                s.marks[j] = 0.0f;
            if (s.marks[j] > 100.0f)
                s.marks[j] = 100.0f;
            s.total += s.marks[j];
        }
        s.average = s.total / numSubjects;
        s.grade = computeGrade(s.average);

        if (count < 1000)
        {
            students[count++] = s;
            imported++;
        }

        free(row);
    }

    free(header);
    fclose(csv);

    if (!saveStudents(students, count))
    {
        printf("Failed to save students to data file after import.\n");
        return;
    }

    printf("Imported %d student(s) from CSV. Total students now: %d\n", imported, count);
}

void addStudent(void)
{
    Student s;
    printf("\n>>> Add New Student <<<\n");
    readString("Enter Registration Number (e.g., 25BCE1123): ", s.regNo, MAX_REG_NO_LEN);
    readString("Enter Name: ", s.name, MAX_NAME_LEN);
    while (s.name[0] == '\0')
    {
        printf("Name cannot be empty. Re-enter Name: ");
        safeReadLine(s.name, MAX_NAME_LEN);
    }
    s.total = 0.0f;
    char prompt[100];
    for (int i = 0; i < numSubjects; i++)
    {
        sprintf(prompt, "Enter marks for %s (0-100): ", subjectNames[i]);
        s.marks[i] = readFloat(prompt, 0.0f, 100.0f);
        s.total += s.marks[i];
    }
    s.average = s.total / numSubjects;
    s.grade = computeGrade(s.average);

    FILE *fp = fopen(DATA_FILE, "a");
    if (!fp)
    {
        printf("Unable to open data file for appending.\n");
        return;
    }
    fprintf(fp, "%s|%s", s.regNo, s.name);
    for (int i = 0; i < numSubjects; i++)
    {
        fprintf(fp, "|%.2f", s.marks[i]);
    }
    fprintf(fp, "|%.2f|%.2f|%c\n", s.total, s.average, s.grade);
    fclose(fp);
    printf("Student added successfully.\n");
}

void searchStudent(void)
{
    printf("\n>>> Search Student <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No students present.\n");
        return;
    }
    char key[MAX_REG_NO_LEN];
    readString("Enter Registration Number to search: ", key, MAX_REG_NO_LEN);
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (strcmp(students[i].regNo, key) == 0)
        {
            found = 1;
            printf("Found: %s | %s\n", students[i].regNo, students[i].name);
            for (int j = 0; j < numSubjects; j++)
            {
                printf("  %s: %.2f\n", subjectNames[j], students[i].marks[j]);
            }
            printf("  Total: %.2f | Avg: %.2f | Grade: %c\n",
                   students[i].total, students[i].average, students[i].grade);
            break;
        }
    }
    if (!found)
    {
        printf("Registration number %s not found.\n", key);
    }
}

void updateStudent(void)
{
    printf("\n>>> Update Student <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No students present.\n");
        return;
    }
    char key[MAX_REG_NO_LEN];
    readString("Enter Registration Number to update: ", key, MAX_REG_NO_LEN);
    int found = 0;
    for (int i = 0; i < count; i++)
    {
        if (strcmp(students[i].regNo, key) == 0)
        {
            found = 1;
            printf("Current: %s | %s\n", students[i].regNo, students[i].name);
            for (int j = 0; j < numSubjects; j++)
            {
                printf("  %s: %.2f\n", subjectNames[j], students[i].marks[j]);
            }
            printf("  Total: %.2f | Avg: %.2f | Grade: %c\n",
                   students[i].total, students[i].average, students[i].grade);
            char prompt[100];
            students[i].total = 0.0f;
            for (int j = 0; j < numSubjects; j++)
            {
                sprintf(prompt, "New marks for %s (0-100): ", subjectNames[j]);
                students[i].marks[j] = readFloat(prompt, 0.0f, 100.0f);
                students[i].total += students[i].marks[j];
            }
            students[i].average = students[i].total / numSubjects;
            students[i].grade = computeGrade(students[i].average);
            if (saveStudents(students, count))
            {
                printf("Student record updated successfully.\n");
            }
            break;
        }
    }
    if (!found)
    {
        printf("Registration number %s not found.\n", key);
    }
}

void displayToppers(void)
{
    printf("\n>>> Display Toppers <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No records found.\n");
        return;
    }
    float highest = students[0].total;
    for (int i = 1; i < count; i++)
    {
        if (students[i].total > highest)
        {
            highest = students[i].total;
        }
    }
    printf("Highest total marks: %.2f\n", highest);
    printf("Topper(s):\n");
    for (int i = 0; i < count; i++)
    {
        if (students[i].total == highest)
        {
            printf("RegNo: %s, Name: %s, Total: %.2f, Avg: %.2f, Grade: %c\n",
                   students[i].regNo, students[i].name, students[i].total,
                   students[i].average, students[i].grade);
        }
    }
}

void classStatistics(void)
{
    printf("\n>>> Class Statistics <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No students present.\n");
        return;
    }
    float sum = 0, highest = students[0].total, lowest = students[0].total;
    for (int i = 0; i < count; i++)
    {
        sum += students[i].total;
        if (students[i].total > highest)
            highest = students[i].total;
        if (students[i].total < lowest)
            lowest = students[i].total;
    }
    printf("Class average total: %.2f\n", sum / count);
    printf("Highest total: %.2f\n", highest);
    printf("Lowest total: %.2f\n", lowest);
}

void gradeDistribution(void)
{
    printf("\n>>> Grade Distribution <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No students present.\n");
        return;
    }
    int a = 0, b = 0, c = 0, f = 0;
    for (int i = 0; i < count; i++)
    {
        if (students[i].grade == 'A')
            a++;
        else if (students[i].grade == 'B')
            b++;
        else if (students[i].grade == 'C')
            c++;
        else if (students[i].grade == 'F')
            f++;
    }
    printf("A: %d, B: %d, C: %d, Fail: %d\n", a, b, c, f);
}

void displayAllStudents(void)
{
    printf("\n>>> All Student Records <<<\n");
    Student students[1000];
    int count = loadStudents(students, 1000);
    if (count == 0)
    {
        printf("No students present.\n");
        return;
    }
    for (int i = 0; i < count; i++)
    {
        printf("RegNo: %s | Name: %s\n", students[i].regNo, students[i].name);
        for (int j = 0; j < numSubjects; j++)
        {
            printf("  %s: %.2f\n", subjectNames[j], students[i].marks[j]);
        }
        printf("  Total: %.2f | Avg: %.2f | Grade: %c\n",
               students[i].total, students[i].average, students[i].grade);
    }
}

int main(void)
{
    loadSubjects();

#ifdef __EMSCRIPTEN__
    // For Emscripten/Web version, just initialize and return
    // The HTML interface handles all user interaction
    printf("Student Management System loaded successfully.\n");
    printf("Number of subjects: %d\n", numSubjects);
    for (int i = 0; i < numSubjects; i++)
    {
        printf("Subject %d: %s\n", i + 1, subjectNames[i]);
    }
    return 0;
#else
    // For terminal version, show interactive menu
    int choice;
    while (1)
    {
        printf("\n=== Smart Student Performance Analyzer ===\n");
        printf("1. Add Student\n");
        printf("2. Search Student\n");
        printf("3. Update Student\n");
        printf("4. Display Toppers\n");
        printf("5. Class Statistics\n");
        printf("6. Grade Distribution\n");
        printf("7. Display All Students\n");
        printf("8. Configure Subjects\n");
        printf("9. Display Subject Configuration\n");
        printf("10. Import Students from CSV\n");
        printf("0. Exit\n");
        choice = readInt("Enter your choice: ");
        switch (choice)
        {
        case 1:
            addStudent();
            break;
        case 2:
            searchStudent();
            break;
        case 3:
            updateStudent();
            break;
        case 4:
            displayToppers();
            break;
        case 5:
            classStatistics();
            break;
        case 6:
            gradeDistribution();
            break;
        case 7:
            displayAllStudents();
            break;
        case 8:
            configureSubjects();
            break;
        case 9:
            displaySubjectConfig();
            break;
        case 10:
            importFromCSV();
            break;
        case 0:
            printf("Exiting program. Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice. Please select a valid option.\n");
        }
    }
    return 0;
#endif
}
