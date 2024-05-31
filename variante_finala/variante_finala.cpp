#include <iostream>
#include <vector>
#include <chrono>
#include <random>
using namespace std;
#include <cassert>

//void assert(bool ok)
//{
//    if (!ok)
//    {
//        cout << "assertion failed!\n";
//        exit(0);
//    }
//}

mt19937 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

long long getrnginteger(long long l, long long r)
{
    uniform_int_distribution<> distrib(l, r);
    return distrib(rng);
}

float getrngfloat(float l, float r)
{
    uniform_real_distribution<float> distrib(l, r);
    return distrib(rng);
}

vector<int> generateTest(int n, int maxdim)
{
    n = getrnginteger(1, n);
    maxdim = getrnginteger(0, maxdim);
    vector<int> v(n);
    for (auto& x : v) x = getrnginteger(0, maxdim);
    return v;
}

vector<float> generateFloatTest(int n, float maxdim)
{
    n = getrnginteger(1, n);
    maxdim = getrnginteger(0, maxdim);
    vector<float> v(n);
    for (auto& x : v) x = getrngfloat(0, maxdim);
    return v;
}


double timeLimit = 1;
time_t bg;
bool TLE;

void resetTime()
{
    TLE = false;
    bg = clock();
}

double getElapsedTime()
{
    time_t nd = clock();
    return ((double)(nd - bg)) / CLOCKS_PER_SEC;
}

#define CHECKQUIT {if (getElapsedTime() >= timeLimit) {TLE = true; return;}}

template<typename T>
class Sort
{
public:
    int fail;
    int inTime;
    double sumTime;
    Sort()
    {
        fail = 0;
        inTime = 0;
        sumTime = 0;
    }
    virtual void sortref(vector<T>& v) = 0;
    vector<T> sort(vector<T> v)
    {
        shuffle(v.begin(), v.end(), rng);
        sortref(v);
        return v;
    }
    virtual string getid() = 0;
};


template<typename T>
class RadixSort : public Sort<T>
{
private:
    T baza;
public:
    RadixSort(T baza) : baza(baza) {}

    void sortref(vector<T>& v) override
    {
        T maxi = 0;
        for (int i = 0; i < v.size(); i++)
            if (v[i] > maxi)
                maxi = v[i];
        T putere = 0;
        while (maxi > 0)
        {
            maxi /= baza;
            putere++;
        }
        T p = 1;
        for (int poz = 0; poz < putere; poz++)
        {
            CHECKQUIT;
            vector<vector<T>> cine(baza);
            for (int i = 0; i < v.size(); i++)
            {
                cine[v[i] / p % baza].push_back(v[i]);
            }
            v.clear();
            for (int i = 0; i < baza; i++)
            {
                for (int j = 0; j < cine[i].size(); j++)
                    v.push_back(cine[i][j]);
            }
            p *= baza;
        }
    }

    string getid() override { return "RadixSort"; }
};

template<typename T>
class MergeSort : public Sort<T>
{
public:
    MergeSort()
    {

    }
    void mergesort(int st, int dr, vector<T>& v)
    {
        CHECKQUIT;
        if (st == dr) return;
        int mij = (st + dr) / 2;
        mergesort(st, mij, v);
        mergesort(mij + 1, dr, v);
        vector<T> a, b;
        for (int i = st; i <= mij; i++)
            a.push_back(v[i]);
        for (int j = mij + 1; j <= dr; j++)
            b.push_back(v[j]);
        vector<T> v2;
        int i = 0;
        int j = 0;
        while (i < a.size() && j < b.size())
        {
            if (a[i] < b[j])
                v2.push_back(a[i++]);
            else
                v2.push_back(b[j++]);
        }
        if (i < a.size())
        {
            while (i < a.size())
                v2.push_back(a[i++]);
        }
        if (j < b.size())
        {
            while (j < b.size())
                v2.push_back(b[j++]);
        }
        for (int i = st; i <= dr; i++)
        {
            v[i] = v2[i - st];
        }
    }
    void sortref(vector<T>& v) override
    {
        mergesort(0, v.size() - 1, v);
    }

    string getid() override { return "MergeSort"; }
};


template<typename T>
class ShellSort : public Sort<T>
{
private:
    vector<int> gaps;

public:
    ShellSort(vector<int> gaps) : gaps(gaps)
    {

    }
    void sortref(vector<T>& v) override
    {
        for (auto& gap : gaps)
        {
            CHECKQUIT;
            for (int i = gap; i < v.size(); i++)
            {
                CHECKQUIT;
                int j = i;
                while (j - gap >= 0 && v[j] < v[j - gap])
                {
                    swap(v[j], v[j - gap]);
                    j -= gap;
                }
            }
        }
    }

    string getid() override { return "ShellSort"; }
};

template<typename T>
class HeapSort : public Sort<T>
{
    void heap_up(vector<T>& v, int i)
    {
        if (i == 1) return;
        if (v[i] < v[i / 2])
        {
            swap(v[i], v[i / 2]);
            heap_up(v, i / 2);
        }
    }
    void heap_down(vector<T>& v, int i)
    {
        if (2 * i >= v.size())
            return;
        if (2 * i + 1 >= v.size())
        {
            assert(2 * i < v.size());
            if (v[i] > v[2 * i])
            {
                swap(v[i], v[2 * i]);
                heap_down(v, 2 * i);
            }
            return;
        }
        int c1 = 2 * i;
        int c2 = 2 * i + 1;
        if (v[i] <= v[c1] && v[i] <= v[c2]) return;
        assert(c1 < v.size());
        assert(c2 < v.size());
        if (v[c1] < v[c2])
        {
            swap(v[i], v[c1]);
            heap_down(v, 2 * i);
        }
        else
        {
            swap(v[i], v[c2]);
            heap_down(v, 2 * i + 1);
        }
    }

public:
    void sortref(vector<T>& v)
    {
        CHECKQUIT;
        vector<T> heap;
        heap.push_back(0);
        for (int i = 0; i < v.size(); i++)
        {
            heap.push_back(v[i]);
            heap_up(heap, heap.size() - 1);
        }
        int step = v.size();
        v.clear();
        int cnt = 0;
        for (int jj = 1; jj <= step; jj++)
        {
            cnt++;
            v.push_back(heap[1]);
            swap(heap[1], heap[heap.size() - 1]);
            heap.pop_back();
            heap_down(heap, 1);
        }
    }
    string getid() override { return "HeapSort"; }
};

template<typename T>
class QuickSort : public Sort<T>
{
private:
    void split(vector<T>& v, vector<T>& sub, vector<T>& egal, vector<T>& peste, T piv)
    {
        sub.clear();
        egal.clear();
        peste.clear();
        for (auto& x : v)
        {
            if (abs(x - piv) < 1e-8)
            {
                egal.push_back(x);
                continue;
            }
            if (x < piv)
                sub.push_back(x);
            else
                peste.push_back(x);
        }
    }
public:
    void sortref(vector<T>& v)
    {
        CHECKQUIT;
        T piv = v[0];
        vector<T> sub;
        vector<T> egal;
        vector<T> peste;
        split(v, sub, egal, peste, piv);
        if (sub.empty() && peste.empty())
        {
            v = egal;
            return;
        }
        if (sub.empty() != 0)
        {
            for (auto& x : egal)
                sub.push_back(x);
        }
        else
        {
            for (auto& x : egal)
                peste.push_back(x);
        }
        assert(!sub.empty() && !peste.empty());
        assert(sub.size() + peste.size() == v.size());
        sortref(sub);
        sortref(peste);
        v.clear();
        for (auto& x : sub)
            v.push_back(x);
        for (auto& x : peste)
            v.push_back(x);
    }

    string getid() override { return "QuickSort"; }
};

template<typename T>
class InsertionSort : public Sort<T>
{
public:
    void sortref(vector<T>& v) override
    {
        int n = (int)v.size();
        for (int i = 0; i < n; i++)
        {
            CHECKQUIT;
            int j = i;
            while (j - 1 >= 0 && v[j - 1] > v[j])
            {
                swap(v[j - 1], v[j]);
                j--;
            }
        }
    }

    string getid() override { return "InsertionSort"; }
};

template<typename T>
class SortDeLaEi : public Sort<T>
{
public:
    void sortref(vector<T>& v) override
    {
        sort(v.begin(), v.end());
    }

    string getid() override { return "SortDeLaEi"; }
};

int main()
{
    if (1)
    {
        vector<int> Gaps1;
        for (int i = 30; i >= 0; i--) Gaps1.push_back(1 << i);
        vector<Sort<int>*> sortings;
        sortings.push_back(new RadixSort<int>(10));
        sortings.push_back(new MergeSort<int>());
        sortings.push_back(new ShellSort<int>(Gaps1));
        sortings.push_back(new HeapSort<int>());
        sortings.push_back(new QuickSort<int>());
        sortings.push_back(new InsertionSort<int>());
        sortings.push_back(new SortDeLaEi<int>());
        for (long long tc = 1; 1; tc++)
        {
            resetTime();
            vector<int> v = generateTest((int)1e6, (int)1e9);
            vector<vector<int>> outs;
            double sum = 0;
            for (auto& sort : sortings)
            {
                resetTime();
                outs.push_back(sort->sort(v));
                if (TLE)
                {
                    sort->fail++;
                    outs.pop_back();
                }
                else
                {
                    sort->inTime++;
                    sort->sumTime += getElapsedTime();
                }
                //cout << sort->getid() << " ";
                sum += getElapsedTime();
            }
            for (auto& sort : sortings)
            {
                cout << sort->getid() << " : " << sort->fail << " " << sort->inTime << " " << (sort->sumTime + sort->fail * timeLimit) / (sort->fail + sort->inTime) << "\n";
            }
            cout << "----------------------------------\n";
            //cout << "\n";

            //cout << "\n";
            for (int i = 1; i < (int)outs.size(); i++)
            {
                if (outs[i] != outs[i - 1])
                {
                    //cout << "wrong i = " << i << "\n";
                    exit(0);
                }
                assert(outs[i] == outs[i - 1]);
            }
            //cout << "done " << tc << ", am calculat " << (int)outs.size() << " algoritmi diferiti de sortare, a durat " << sum << " secunde" << "\n";
        }
    }
    if (0)
    {
        vector<int> Gaps1;
        for (int i = 30; i >= 0; i--) Gaps1.push_back(1 << i);
        vector<Sort<float>*> sortings;
        sortings.push_back(new MergeSort<float>());
        sortings.push_back(new ShellSort<float>(Gaps1));
        sortings.push_back(new HeapSort <float>());
        sortings.push_back(new QuickSort<float>());
        sortings.push_back(new InsertionSort<float>());
        sortings.push_back(new SortDeLaEi<float>());
        for (long long tc = 1; 1; tc++)
        {
            resetTime();
            vector<float> v = generateFloatTest((int)1e6, (int)1e9);
            vector<vector<float>> outs;
            double sum = 0;
            for (auto& sort : sortings)
            {
                resetTime();
                outs.push_back(sort->sort(v));
                if (TLE)
                {
                    sort->fail++;
                    outs.pop_back();
                }
                else
                {
                    sort->inTime++;
                    sort->sumTime += getElapsedTime();
                }
                //cout << sort->getid() << " ";
                sum += getElapsedTime();
            }
            for (auto& sort : sortings)
            {
                cout << sort->getid() << " : " << sort->fail << " " << sort->inTime << " " << (sort->sumTime + sort->fail * timeLimit) / (sort->fail + sort->inTime) << "\n";
            }
            cout << "----------------------------------\n";
            //cout << "\n";

            //cout << "\n";
            for (int i = 1; i < (int)outs.size(); i++)
            {
                if (outs[i] != outs[i - 1])
                {
                    //cout << "wrong i = " << i << "\n";
                    exit(0);
                }
                assert(outs[i] == outs[i - 1]);
            }
            //cout << "done " << tc << ", am calculat " << (int)outs.size() << " algoritmi diferiti de sortare, a durat " << sum << " secunde" << "\n";
        }
    }
}

/*
radix, merge, shell, heap, quick, insertion
*/