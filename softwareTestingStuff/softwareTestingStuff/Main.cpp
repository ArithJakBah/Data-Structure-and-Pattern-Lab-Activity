// SWE30009 Assignment 2 - Mutation testing of MR1 and MR2
// Program P: LLM API gateway bill calculator (Use Case 1 and Use Case 2)
// Standard library functions are deliberately avoided in the logic (only printf for output),
// so that there are enough statements available to mutate.

#include <cstdio>

// ======================= Program P (original) =======================
double programP(const double prices[], int n)
{
    double subtotal = 0.0;                      // S1
    for (int i = 0; i < n; i++)                 // S2
    {
        subtotal = subtotal + prices[i];        // S3
    }
    double total = subtotal * 1.06;             // S4  (6% SST)
    return total;                               // S5
}

// ======================= Mutants (each has ONE fault) =======================
// M1: S4 modified - tax constant 1.06 changed to 1.05
double mutantM1(const double prices[], int n)
{
    double subtotal = 0.0;
    for (int i = 0; i < n; i++)
    {
        subtotal = subtotal + prices[i];
    }
    double total = subtotal * 1.05;             // MUTATED (was 1.06)
    return total;
}

// M2: S2 modified - loop condition i < n changed to i < n - 1 (last price skipped)
double mutantM2(const double prices[], int n)
{
    double subtotal = 0.0;
    for (int i = 0; i < n - 1; i++)             // MUTATED (was i < n)
    {
        subtotal = subtotal + prices[i];
    }
    double total = subtotal * 1.06;
    return total;
}

// M3: S1 modified - initial subtotal 0.0 changed to 1.0
double mutantM3(const double prices[], int n)
{
    double subtotal = 1.0;                      // MUTATED (was 0.0)
    for (int i = 0; i < n; i++)
    {
        subtotal = subtotal + prices[i];
    }
    double total = subtotal * 1.06;
    return total;
}

// ======================= Helpers =======================
typedef double (*Program)(const double[], int);

bool nearlyEqual(double a, double b)            // own abs, no <cmath>
{
    double d = a - b;
    if (d < 0) d = -d;
    return d < 0.000001;
}

void printList(const char* label, int idx, const double v[], int n)
{
    printf("%s%d = [", label, idx);
    for (int i = 0; i < n; i++)
    {
        printf("%.2f", v[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

// ======================= Test data (defined manually) =======================
// Source inputs SI1..SI5
const double SI1[] = { 4.50, 13.00, 18.00 };
const double SI2[] = { 0.02 };
const double SI3[] = { 0.50, 1.20, 2.00, 3.80, 5.00, 7.50, 10.00, 15.00, 20.00 };
const double SI4[] = { 1500.00, 2500.50 };
const double SI5[] = { 5.00, 5.00, 5.00 };
const int    N_SI[5] = { 3, 1, 9, 2, 3 };
const double* SI[5] = { SI1, SI2, SI3, SI4, SI5 };
const double EXPECTED[5] = { 37.63, 0.0212, 68.90, 4240.53, 15.90 };

// MR1 follow-up inputs: append k = 5.00
const double FI1_MR1[] = { 4.50, 13.00, 18.00, 5.00 };
const double FI2_MR1[] = { 0.02, 5.00 };
const double FI3_MR1[] = { 0.50, 1.20, 2.00, 3.80, 5.00, 7.50, 10.00, 15.00, 20.00, 5.00 };
const double FI4_MR1[] = { 1500.00, 2500.50, 5.00 };
const double FI5_MR1[] = { 5.00, 5.00, 5.00, 5.00 };
const int    N_FI_MR1[5] = { 4, 2, 10, 3, 4 };
const double* FI_MR1[5] = { FI1_MR1, FI2_MR1, FI3_MR1, FI4_MR1, FI5_MR1 };
const double K = 5.00;

// MR2 follow-up inputs: every element * 0.5
const double FI1_MR2[] = { 2.25, 6.50, 9.00 };
const double FI2_MR2[] = { 0.01 };
const double FI3_MR2[] = { 0.25, 0.60, 1.00, 1.90, 2.50, 3.75, 5.00, 7.50, 10.00 };
const double FI4_MR2[] = { 750.00, 1250.25 };
const double FI5_MR2[] = { 2.50, 2.50, 2.50 };
const int    N_FI_MR2[5] = { 3, 1, 9, 2, 3 };
const double* FI_MR2[5] = { FI1_MR2, FI2_MR2, FI3_MR2, FI4_MR2, FI5_MR2 };

// ======================= Task 4: verify Program P =======================
void verifyProgramP()
{
    printf("*** Program P: Verification of SI1 to SI5 ***\n");
    for (int t = 0; t < 5; t++)
    {
        double actual = programP(SI[t], N_SI[t]);
        printList("SI", t + 1, SI[t], N_SI[t]);
        printf("Expected = %.4f, Actual = %.4f -> %s\n\n",
            EXPECTED[t], actual, nearlyEqual(actual, EXPECTED[t]) ? "PASS" : "FAIL");
    }
}

// ======================= Task 7: run MTGs =======================
// Returns the number of MTGs in which the MR was violated.
int runMR1(const char* name, Program prog)
{
    int violations = 0;
    printf("*** Program %s: Testing MTGs for MR1 ***\n", name);
    for (int t = 0; t < 5; t++)
    {
        double so = prog(SI[t], N_SI[t]);
        double fo = prog(FI_MR1[t], N_FI_MR1[t]);
        bool violated = !nearlyEqual(fo, so + K * 1.06);   // MR1: FO = SO + k*1.06
        if (violated) violations++;
        printf("MTG%d\n", t + 1);
        printList("SI", t + 1, SI[t], N_SI[t]);
        printf("SO%d = %.4f\n", t + 1, so);
        printList("FI", t + 1, FI_MR1[t], N_FI_MR1[t]);
        printf("FO%d = %.4f\n\n", t + 1, fo);
    }
    printf("\n");
    return violations;
}

int runMR2(const char* name, Program prog)
{
    int violations = 0;
    printf("*** Program %s: Testing MTGs for MR2 ***\n", name);
    for (int t = 0; t < 5; t++)
    {
        double so = prog(SI[t], N_SI[t]);
        double fo = prog(FI_MR2[t], N_FI_MR2[t]);
        bool violated = !nearlyEqual(fo, so * 0.5);        // MR2: FO = SO * 0.5
        if (violated) violations++;
        printf("MTG%d\n", t + 1);
        printList("SI", t + 1, SI[t], N_SI[t]);
        printf("SO%d = %.4f\n", t + 1, so);
        printList("FI", t + 1, FI_MR2[t], N_FI_MR2[t]);
        printf("FO%d = %.4f\n\n", t + 1, fo);
    }
    printf("\n");
    return violations;
}

int main()
{
    verifyProgramP();

    runMR1("M1", mutantM1);
    runMR1("M2", mutantM2);
    runMR1("M3", mutantM3);
    runMR2("M1", mutantM1);
    runMR2("M2", mutantM2);
    runMR2("M3", mutantM3);
    return 0;
}