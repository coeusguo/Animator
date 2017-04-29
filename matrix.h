#ifndef MAT_H
#define MAT_H


#include <iostream>
using namespace std;

template <typename T = float>
class Mat
{
public:
    Mat(int M,int N);
    Mat(T* data,int M,int N);
    ~Mat();
    T* data();
    T at(int i, int j);
    void set(int i, int j, T v);
    T determinant();
    Mat<T> transposed();
    Mat<T> inverse();
    Mat<T> cofactor(int i, int j);
    void printData();
    void printData(T* data, int m, int n);
    void printData(T* data, int dim);
	int M;
	int N;
private:
    T* m_data;
    T* minorData(T* olddata, int m, int n, int dim);
    T determinantHelper(T* data, int dim);
};

template <typename T>
Mat<T> operator + (Mat<T> &m1, Mat<T> &m2)
{
	int M = m1.M;
	int N = m1.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = (m1.data())[i]+(m2.data())[i];
    return (Mat<T>(data,M,N));
}

template <typename T>
Mat<T> operator - (Mat<T> &m1, Mat<T> &m2)
{
	int M = m1.M;
	int N = m1.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = (m1.data())[i]-(m2.data())[i];
    return (Mat<T>(data,M,N));
}

template < typename T>
Mat<T> operator - (Mat<T> &m)
{
	int M = m1.M;
	int N = m1.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = -(m.data())[i];
    return Mat<T>(data,M,N);
}

template <typename T>
Mat<T> operator * (Mat<T> m1, Mat<T> m2)
{
	int M = m1.M;
	int K = m1.N;
	int N = m2.N;
	//cout << "M:" << M << ",K:" << K << ",N:" << N << endl;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++)
        {
            T v = 0;
            T* d1 = m1.data();
            T* d2 = m2.data();
            for(int k = 0; k < K; k++)
            {
                v += d1[i*K+k]*d2[k*N+j];
            }
            data[i*N+j] = v;
        }
    return (Mat<T>(data,M,N));
}

template <typename T>
Mat<T> operator * (Mat<T> &m, T v)
{
	int M = m.M;
	int N = m.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = (m.data())[i]*v;
    return (Mat<T>(data,M,N));
}

template <typename T>
Mat<T> operator * (T v, Mat<T> &m)
{
	int M = m.M;
	int N = m.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = (m.data())[i]*v;
    return (Mat<T>(data,M,N));
}

template <typename T>
Mat<T> operator / (Mat<T> &m, T v)
{
	int M = m.M;
	int N = m.N;
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M*N; i++)
        data[i] = (m.data())[i]/v;
    return (Mat<T>(data,M,N));
}

template <typename T>
void Mat<T>::printData()
{

    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
            cout<<m_data[i*N+j]<<'\t';
        cout<<endl;
    }
    cout<<endl;
}

template <typename T>
void Mat<T>::printData(T *data, int m, int n)
{
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
            cout<<data[i*n+j]<<'\t';
        cout<<endl;
    }
    cout<<endl;
}

template <typename T>
void Mat<T>::printData(T *data, int dim)
{
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0; j < dim; j++)
            cout<<data[i*dim+j]<<'\t';
        cout<<endl;
    }
    cout<<endl;
}

template <typename T>
Mat<T>::Mat(int M,int N)
{
    if(M<1 || N<1)
        throw "Error: M or N can't be less than 1.";
    //m_data = (T*)calloc(M*N,sizeof(T));
    m_data = new T[M*N];
	this->M = M;
	this->N = N;
}

template <typename T>
Mat<T>::Mat(T *data,int M,int N)
{
    if(M<1 || N<1)
        throw "Error: M or N can't be less than 1.";
    //m_data = (T*)malloc(M*N*sizeof(T));
    m_data = new T[M*N];
    for(int i = 0; i < M*N; i++)
        m_data[i] = data[i];
	this->M = M;
	this->N = N;
}

template <typename T>
Mat<T>::~Mat()
{

}

template <typename T>
T* Mat<T>::data()
{
    return m_data;
}

template <typename T>
T Mat<T>::at(int i, int j)
{
    return (m_data[i*N+j]);
}

template <typename T>
void Mat<T>::set(int i, int j, T v)
{
    m_data[i*N+j] = v;
}

template < typename T>
T Mat<T>::determinant()
{
    if(M!=N)
        return 0;
    return determinantHelper(m_data, M);
}

template <typename T>
T Mat<T>::determinantHelper(T *data, int dim)
{
    if(dim == 1)
        return data[0];
    if(dim == 2)
        return data[0]*data[3]-data[1]*data[2];
    T deter = 0;
    for(int i = 0; i < dim; i++)
    {
        T* minData = minorData(data, 0, i, dim);
        deter += data[i]*pow(-1,i)*determinantHelper(minData, dim-1);
    }
    return deter;
}

template <typename T>
Mat<T> Mat<T>::transposed()
{
    T* newData = (T*)calloc(N*M, sizeof(T));
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            newData[i*M+j] = m_data[j*N+i];
    return (Mat<T>(newData,N,M));
}

template <typename T>
Mat<T> Mat<T>::inverse()
{
    if(M!=N)
        throw "Error: M!=N";
    T deter = determinant();
    if(deter == 0)
    {
        cout<<"Error: matrix with determinant=0 has no inverse!\n";
        printData();
        throw "Error: matrix with determinant=0 has no inverse!\n";
    }
    if(M == 1)
    {
        Mat<T> ret = Mat<T>(1,1);
        ret.set(0, 0, 1.0f/m_data[0]);
        return ret;
    }
    if(M == 2)
    {
        Mat<T> ret = Mat<T>(2,2);
        ret.set(0, 0, m_data[3]/(deter));
        ret.set(0, 1, m_data[2]/(-deter));
        ret.set(1, 0, m_data[1]/(-deter));
        ret.set(1, 1, m_data[0]/(deter));
        return ret;
    }
    T* data = (T*)malloc(M*N*sizeof(T));
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++)
            data[i*N+j] = pow(-1,i+j)*(cofactor(i, j).determinant())/(deter);
    return Mat<T>(data,M,N).transposed();
}

template <typename T>
Mat<T> Mat<T>::cofactor(int i, int j)
{
    T* data = minorData(m_data, i, j, M);
    Mat<T> ret = Mat<T>(data,M - 1,N - 1);
    return ret;
}


template <typename T>
T* Mat<T>::minorData(T* olddata, int m, int n, int dim)
{
    T* data = (T*)calloc((dim-1)*(dim-1), sizeof(T));
    int r = 0;
    int c = 0;
    for(int i = 0; i < dim; i++)
    {
        if(i == m) continue;
        c = 0;
        for(int j = 0; j < dim; j++)
        {
            if(j == n) continue;
            data[r*(dim-1)+c] = olddata[i*(dim)+j];
            c++;
        }
        r++;
    }
    return data;
}


#endif // MAT_H
