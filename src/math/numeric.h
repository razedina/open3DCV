#ifndef open3DCV_numeric_h
#define open3DCV_numeric_h

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/SparseCore>
#include <Eigen/SVD>
#include <Eigen/StdVector>

#include <cmath>
#include <numeric>
#include <string>
#include <iostream>
#include <vector>

namespace open3DCV
{

// Check MSVC
#if _WIN32 || _WIN64
  #if _WIN64
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

// Check GCC
#if __GNUC__
  #if __x86_64__ || __ppc64__ || _LP64
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

typedef Eigen::Vector2i Vec2i;
typedef Eigen::Vector2f Vec2f;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector4i Vec4i;
typedef Eigen::Matrix<float, 6, 1> Vec6f;
typedef Eigen::Matrix<float, 9, 1> Vec9f;
typedef Eigen::Matrix<double, 9, 1> Vec9;

#if defined(ENV32BIT)
    typedef Eigen::Matrix<double, 2, 1, Eigen::DontAlign> Vec2;
    typedef Eigen::Matrix<float, 4, 1, Eigen::DontAlign> Vec4f;
    typedef Eigen::Matrix<double, 4, 1, Eigen::DontAlign> Vec4;
    typedef Eigen::Matrix<double, 6, 1, Eigen::DontAlign> Vec6;
    typedef Eigen::Matrix<float, 8, 1, Eigen::DontAlign> Vec8f;
    typedef Eigen::Matrix<double, 8, 1, Eigen::DontAlign> Vec8;
#else
    typedef Eigen::Vector2d Vec2;
    typedef Eigen::Vector4f Vec4f;
    typedef Eigen::Vector4d Vec4;
    typedef Eigen::Matrix<double, 6, 1> Vec6;
    typedef Eigen::Matrix<float, 8, 1> Vec8f;
    typedef Eigen::Matrix<double, 8, 1> Vec8;
#endif

typedef Eigen::Matrix2i Mat2i;
typedef Eigen::Matrix3i Mat3i;
typedef Eigen::Matrix3f Mat3f;
typedef Eigen::Matrix3d Mat3;

#if defined(ENV32BIT)
    typedef Eigen::Matrix<float, 2, 2, Eigen::DontAlign> Mat2f;
    typedef Eigen::Matrix<double, 2, 2, Eigen::DontAlign> Mat2;
    typedef Eigen::Matrix<int, 4, 4, Eigen::DontAlign> Mat4i;
    typedef Eigen::Matrix<float, 4, 4, Eigen::DontAlign> Mat4f;
    typedef Eigen::Matrix<double, 4, 4, Eigen::DontAlign> Mat4;
    typedef Eigen::Matrix<float, 3, 4, Eigen::DontAlign> Mat34f;
    typedef Eigen::Matrix<double, 3, 4, Eigen::DontAlign> Mat34;
#else
    typedef Eigen::Matrix2f Mat2f;
    typedef Eigen::Matrix2d Mat2;
    typedef Eigen::Matrix4i Mat4i;
    typedef Eigen::Matrix4f Mat4f;
    typedef Eigen::Matrix4d Mat4;
    typedef Eigen::Matrix<float, 3, 4> Mat34f;
    typedef Eigen::Matrix<double, 3, 4> Mat34;
#endif

//-- General purpose Matrix and Vector
typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> Vecu;
typedef Eigen::VectorXi Veci;
typedef Eigen::VectorXf Vecf;
typedef Eigen::VectorXd Vec;
typedef Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> Matu;
typedef Eigen::MatrixXi Mati;
typedef Eigen::MatrixXf Matf;
typedef Eigen::MatrixXd Mat;
typedef Eigen::Matrix<float, 2, Eigen::Dynamic> Mat2Xf;
typedef Eigen::Matrix<float, Eigen::Dynamic, 2> MatX2f;
typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Mat3Xf;
typedef Eigen::Matrix<float, 4, Eigen::Dynamic> Mat4Xf;
typedef Eigen::Matrix<float, 9, Eigen::Dynamic> Mat9Xf;
typedef Eigen::Matrix<double, 2, Eigen::Dynamic> Mat2X;
typedef Eigen::Matrix<double, 3, Eigen::Dynamic> Mat3X;
typedef Eigen::Matrix<double, 4, Eigen::Dynamic> Mat4X;
typedef Eigen::Matrix<double, 9, Eigen::Dynamic> Mat9X;

/// Quaternion type
typedef Eigen::Quaternion<double> Quaternion;
typedef Eigen::Quaternion<float> Quaternionf;

using Eigen::Map;

/// Trait used for double type
typedef Eigen::NumTraits<double> EigenDoubleTraits;

/// 3x3 matrix using double internal format with RowMajor storage
typedef Eigen::Matrix<double, 3, 3, Eigen::RowMajor> RMat3;

//-- Sparse Matrix (Column major, and row major)
/// Sparse unconstrained matrix using double internal format
typedef Eigen::SparseMatrix<double> sMat;

/// Sparse unconstrained matrix using double internal format and Row Major storage
typedef Eigen::SparseMatrix<double, Eigen::RowMajor> sRMat;


//--------------
//-- Function --
//--------------
template<typename TMat, typename TVec>
TMat cross_product_matrix(const TVec& x)
{
    TMat X;
    X << 0,     -x(2),  x(1),
         x(2),   0,    -x(0),
        -x(1),   x(0),  0;
    return X;
}

/**
 * @brief Compute R(diagonal), Q(orthogonal)
 * @tparam T Type of the number to decompose
 * @param M Input matrix
 * @return void
 * current implementation might be wrong
 */
template<typename T>
inline int rq(T M, T& R, T& Q)
{
    Eigen::FullPivLU<T> lu_decomp(M);
    if(lu_decomp.rank() != 3)
    {
        std::cerr << "Rank of M is not 3, cannot proceed to decompose" << std::endl;
        return 1;
    }
    Eigen::HouseholderQR<T> qr(M.rowwise().reverse().transpose());
    Q = qr.householderQ();
    R = M * Q.inverse();
    Eigen::FullPivLU<T> lu_decompR(R);
    R = R.rowwise().reverse().transpose().eval();
    Q = Q.transpose().eval();
    
    T W;
//    W = R.unaryExpr(std::ptr_fun(sign_func));compute the sign
    R = R * W;
    Q = W * R;
    
    return 0;
}
    
template <typename TMat, typename TVec>
float nullspace(TMat *A, TVec *x)
{
    if ( A->rows() >= A->cols() )
    {
        Eigen::JacobiSVD<TMat> svd( *A, Eigen::ComputeFullV );
        ( *x ) = svd.matrixV().col( A->cols() - 1 );
        return svd.singularValues()( A->cols() - 1 );
    }
    // Extend A with rows of zeros to make it square. It's a hack, but is
    // necessary until Eigen supports SVD with more columns than rows.
    TMat A_extended( A->cols(), A->cols() );
    A_extended.block( A->rows(), 0, A->cols() - A->rows(), A->cols() ).setZero();
    A_extended.block( 0, 0, A->rows(), A->cols() ) = ( *A );
    return nullspace( &A_extended, x );
}
    
template <typename T>
T nullspace(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* A,
            Eigen::Matrix<T, Eigen::Dynamic, 1>* x)
{
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> TMat;
    typedef Eigen::Matrix<T, Eigen::Dynamic, 1> TVec;
    if (A->rows() >= A->cols())
    {
        Eigen::JacobiSVD<TMat> svd(*A, Eigen::ComputeFullV);
        (*x) = svd.matrixV().col(A->cols() - 1);
        return (T)svd.singularValues()(A->cols() - 1);
    }
    // Extend A with rows of zeros to make it square. It's a hack, but is
    // necessary until Eigen supports SVD with more columns than rows.
    TMat A_extended(A->cols(), A->cols());
    A_extended.block<A->cols() - A->rows(), A->cols()>(A->rows(), 0).setZero();
    A_extended.block<A->rows(), A->cols()>(0, 0) = (*A);
    return nullspace(&A_extended, x);
}


template <typename T>
inline double Nullspace2(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> *A,
                         Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> *x1,
                         Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> *x2 )
{
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> TMat;
    typedef Eigen::Matrix<T, Eigen::Dynamic, 1> TVec;
    if ( A->rows() >= A->cols() )
    {
        Eigen::JacobiSVD<TMat> svd(*A, Eigen::ComputeFullV);
        TMat V = svd.matrixV();
        *x1 = V.col(A->cols() - 1);
        *x2 = V.col(A->cols() - 2);
        return svd.singularValues()(A->cols() - 1);
    }
    // Extend A with rows of zeros to make it square. It's a hack, but is
    // necessary until Eigen supports SVD with more columns than rows.
    TMat A_extended(A->cols(), A->cols());
    A_extended.block<A->cols() - A->rows(), A->cols()>(A->rows(), 0).setZero();
    A_extended.block<A->rows(), A->cols()>(0, 0) = (*A);
    return Nullspace2(&A_extended, x1, x2);
}

    
template <typename T>
void svd(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* A,
         Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* U,
         Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* S,
         Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>* V)
{
    
}

template <typename Mat0, typename Mat1, typename Mat2>
void svd(Mat0 *A, Mat1 *U, Mat2 *S, Mat2 *V)
{
    if (A->rows() >= A->cols())
    {
        Eigen::JacobiSVD<Mat0> asvd(*A, Eigen::ComputeFullU | Eigen::ComputeFullV);
        if (U)
        {
            (*U) = asvd.matrixU();
        }
        if (S)
        {
            (*S) = asvd.singularValues().diagonal();
        }
        if (V)
        {
            (*V) = asvd.matrixV();
        }
    }
    else
    {
        Mat1 A_extended(A->cols(), A->cols());
        A_extended.block<A->rows(), A->cols()>(0, 0) = (*A);
        A_extended.block<A->cols() - A->rows(), A->cols()>(A->rows(), 0).setZero();
        svd(&A_extended, U, S, V);
    }
}

template<typename T>
inline T square( T x )
{
  return x * x;
}


/**
* @brief Clamp value inside a given range
* @tparam T working type
* @param val Value to clamp
* @param min Lower bound of clamping range
* @param max Upper bound od clamping range
* @return clamped value
* @note Assuming range form a valid range (ie: min <= max )
*/
template<typename T>
inline T clamp( const T & val, const T& min, const T & max )
{
  return std::max( min, std::min( val, max ) );
  //(val < min) ? val : ((val>max) ? val : max);
}

/**
* @brief Compute mean rotation magnitude of the given rotation matrix
* @param R2 Input rotation matrix
* @return magnitude of the rotation (in radian)
* @note Assuming R2 is a correct rotation matrix
* @note Mean is computed using the matrix column dot products to an Identity matrix
*/
double  getRotationMagnitude( const Mat3 & R2 );

/**
* @brief Gives an indication of the sign of the input value
* @param x Value to test
* @retval 1.0 if value is nul or positive
* @retval -1.0 if value is negative
*/
inline double SIGN( double x )
{
  return x < 0.0 ? -1.0 : 1.0;
}

/**
* @brief Compute L infinity norm
* \f$ \| v \|_{\infty} = \max ( |v_0| , |v_1| , \dots , |v_n| ) \f$
* @param x Input vector
* @return L infinity norm of input vector
*/
template<typename TVec>
inline double NormLInfinity( const TVec &x )
{
  return x.array().abs().maxCoeff();
}

/**
* @brief Compute L infinity distance between two vectors
* @param x first vector
* @param y second vector
* @return distance between input vectors using L infinity norm
*/
template<typename TVec>
inline double DistanceLInfinity( const TVec &x, const TVec &y )
{
  return NormLInfinity( x - y );
}

/**
* @brief Solve linear system
*
* Linear system is given by : \n
* \f$ A x = 0 \f$
* Solution is found using the constraint on x : \f$ \| x \| = 1 \f$
*
* @param[in,out] A Input matrix storing the system to solve
* @param[out] nullspace result vector containing the solution of the system
* @return Singular value corresponding to the solution of the system
*
* @note Computation is made using SVD decomposition of input matrix
* @note Input matrix A content may be modified during computation
* @note Input vector nullspace may be resized to store the full result
*/
//template <typename TMat, typename TVec>
//double Nullsapce( TMat *A, TVec *x )
//{
//  if ( A->rows() >= A->cols() )
//  {
//    Eigen::JacobiSVD<TMat> svd( *A, Eigen::ComputeFullV );
//    ( *x ) = svd.matrixV().col( A->cols() - 1 );
//    return svd.singularValues()( A->cols() - 1 );
//  }
//  // Extend A with rows of zeros to make it square. It's a hack, but is
//  // necessary until Eigen supports SVD with more columns than rows.
//  TMat A_extended( A->cols(), A->cols() );
//  A_extended.block( A->rows(), 0, A->cols() - A->rows(), A->cols() ).setZero();
//  A_extended.block( 0, 0, A->rows(), A->cols() ) = ( *A );
//  return Nullsapce( &A_extended, x );
//}

/**
* @brief Solve linear system and gives the two best solutions
*
* Linear system is given by : \n
* \f$ A x = 0 \f$
* Solution is found using the constraint on x : \f$ \| x \| = 1 \f$
*
* @param[in,out] A Input matrix storing the system to solve
* @param[out] x1 result vector containing the best solution of the system
* @param[out] x2 result vector containing the second best solution of the system
* @return Singular value corresponding to the best solution of the system
*
* @note Computation is made using SVD decomposition of input matrix
* @note Input matrix A content may be modified during computation
* @note Input vector nullspace may be resized to store the full result
*/
template <typename TMat, typename TVec1, typename TVec2>
inline double Nullspace2( TMat *A, TVec1 *x1, TVec2 *x2 )
{
  if ( A->rows() >= A->cols() )
  {
    Eigen::JacobiSVD<TMat> svd( *A, Eigen::ComputeFullV );
    TMat V = svd.matrixV();
    *x1 = V.col( A->cols() - 1 );
    *x2 = V.col( A->cols() - 2 );
    return svd.singularValues()( A->cols() - 1 );
  }
  // Extend A with rows of zeros to make it square. It's a hack, but is
  // necessary until Eigen supports SVD with more columns than rows.
  TMat A_extended( A->cols(), A->cols() );
  A_extended.block( A->rows(), 0, A->cols() - A->rows(), A->cols() ).setZero();
  A_extended.block( 0, 0, A->rows(), A->cols() ) = ( *A );
  return Nullspace2( &A_extended, x1, x2 );
}


/**
* @brief Compute look at matrix
* Make a rotation matrix such that center becomes the direction of the
* positive z-axis, and y is oriented close to up by default.
* @param center New direction (z-axis)
* @param up Desired up vector (y-axis)
* @return Rotation matrix
*/
Mat3 LookAt( const Vec3 &center, const Vec3 & up = Vec3::UnitY() );


/**
* @brief Compute generic look at matrix
* @param eyePosition3D New center of rotation
* @param center3D Position where matrix look at (center3D-eyePosition3D forms the new z-axis)
* @param upVector3D Desired up vector (y-axis)
* @return Rotation matrix conforming the given parameters
*/
Mat3 LookAt2( const Vec3 &eyePosition3D,
              const Vec3 &center3D = Vec3::Zero(),
              const Vec3 &upVector3D = Vec3::UnitY() );

#define SUM_OR_DYNAMIC(x,y) (x==Eigen::Dynamic||y==Eigen::Dynamic)?Eigen::Dynamic:(x+y)

template<typename Derived1, typename Derived2>
struct hstack_return
{
  typedef typename Derived1::Scalar Scalar;
  enum
  {
    RowsAtCompileTime = Derived1::RowsAtCompileTime,
    ColsAtCompileTime = SUM_OR_DYNAMIC( Derived1::ColsAtCompileTime, Derived2::ColsAtCompileTime ),
    Options = Derived1::Flags & Eigen::RowMajorBit ? Eigen::RowMajor : 0,
    MaxRowsAtCompileTime = Derived1::MaxRowsAtCompileTime,
    MaxColsAtCompileTime = SUM_OR_DYNAMIC( Derived1::MaxColsAtCompileTime, Derived2::MaxColsAtCompileTime )
  };
  typedef Eigen::Matrix<Scalar,
          RowsAtCompileTime,
          ColsAtCompileTime,
          Options,
          MaxRowsAtCompileTime,
          MaxColsAtCompileTime> type;
};

template<typename Derived1, typename Derived2>
typename hstack_return<Derived1, Derived2>::type
HStack ( const Eigen::MatrixBase<Derived1>& lhs, const Eigen::MatrixBase<Derived2>& rhs )
{
  typename hstack_return<Derived1, Derived2>::type res;
  res.resize( lhs.rows(), lhs.cols() + rhs.cols() );
  res << lhs, rhs;
  return res;
}


template<typename Derived1, typename Derived2>
struct vstack_return
{
  typedef typename Derived1::Scalar Scalar;
  enum
  {
    RowsAtCompileTime = SUM_OR_DYNAMIC( Derived1::RowsAtCompileTime, Derived2::RowsAtCompileTime ),
    ColsAtCompileTime = Derived1::ColsAtCompileTime,
    Options = Derived1::Flags & Eigen::RowMajorBit ? Eigen::RowMajor : 0,
    MaxRowsAtCompileTime = SUM_OR_DYNAMIC( Derived1::MaxRowsAtCompileTime, Derived2::MaxRowsAtCompileTime ),
    MaxColsAtCompileTime = Derived1::MaxColsAtCompileTime
  };
  typedef Eigen::Matrix<Scalar,
          RowsAtCompileTime,
          ColsAtCompileTime,
          Options,
          MaxRowsAtCompileTime,
          MaxColsAtCompileTime> type;
};

template<typename Derived1, typename Derived2>
typename vstack_return<Derived1, Derived2>::type
VStack ( const Eigen::MatrixBase<Derived1>& lhs, const Eigen::MatrixBase<Derived2>& rhs )
{
  typename vstack_return<Derived1, Derived2>::type res;
  res.resize( lhs.rows() + rhs.rows(), lhs.cols() );
  res << lhs, rhs;
  return res;
}
#undef SUM_OR_DYNAMIC

/**
* @brief Compute Frobenius norm
* \f$ \| A \|_2 = \sqrt{ \sum_{i=1}^n \sum_{j=0}^m a_{ij}^2 } \f$
* @param Input A input matrix
* @return Frobenius norm of given matrix
*/
template<typename TMat>
inline double FrobeniusNorm( const TMat &A )
{
  return sqrt( A.array().abs2().sum() );
}

/**
* @brief Compute distance between two matrices using Frobenius norm
* @param A first matrix
* @param B second matrix
* @return Distance between the input matrices given Frobenius norm
*/
template<typename TMat>
inline double FrobeniusDistance( const TMat &A, const TMat &B )
{
  return FrobeniusNorm( A - B );
}


/**
* @brief Compute similarity of matrices given cosine similarity mesure
* \f$ \cos( A , B ) = \frac{ A . B }{ \| A \|_2 \| B \|_2 } \f$
* @param a First matrix
* @param b Second matrix
* @return cosine similarity mesure between the input matrices
*/
template<class TMat>
double CosinusBetweenMatrices( const TMat &a, const TMat &b )
{
  return ( a.array() * b.array() ).sum() /
         FrobeniusNorm( a ) / FrobeniusNorm( b );
}

/**
* @brief Extract a submatrix given a list of column
* @param A Input matrix
* @param columns A vector of columns index to extract
* @return Matrix containing a subset of input matrix columns
* @note columns index start at index 0
* @note Assuming columns contains a list of valid columns index
*/
template <typename TMat, typename TCols>
TMat ExtractColumns( const TMat &A, const TCols &columns )
{
  TMat compressed( A.rows(), columns.size() );
  for ( size_t i = 0; i < static_cast<size_t>( columns.size() ); ++i )
  {
    compressed.col( i ) = A.col( columns[i] );
  }
  return compressed;
}


/**
* @brief Compute per row mean and variance
* @param A input matrix
* @param[out] mean_pointer a pointer to a vector where mean values are stored
* @param[out] variance_pointer a pointer to a vector where variance values are stored
* @note mean_pointer and variance_pointer vector may be resized to store all values
*/
void MeanAndVarianceAlongRows( const Mat &A,
                               Vec *mean_pointer,
                               Vec *variance_pointer );


/**
* @brief Export a matrix to a file in Text mode
* @param mat Matrix to export
* @param filename Path to the file where matrix will be written
* @param sPrefix Prefix before content of the matrix
* @retval true if export is correct
* @retval false if there was an error during export
*/
bool exportMatToTextFile( const Mat & mat, const std::string & filename,
                          const std::string & sPrefix = "A" );


/**
* @brief Test if value is a finite one
* @param val Input parameter
* @retval 0 if input is not a finite one
* @retval non-zero value if input is a finite one
*/
inline int is_finite( const double val )
{
#ifdef _MSC_VER
  return _finite( val );
#else
  return std::isfinite( val );
#endif
}


/**
* @brief Compute min, max, mean and median value a a given range
* @param begin start computation iterator
* @param end end computation iterator
* @param[out] min Minimum value of range
* @param[out] max Maximum value of range
* @param[out] mean Mean value of range
* @param[out] median Median value of range
* @return true if the statistical values can be estimated
*/
template <typename Type, typename DataInputIterator>
bool minMaxMeanMedian( DataInputIterator begin, DataInputIterator end,
                       Type & min, Type & max, Type & mean, Type & median )
{
  if( std::distance( begin, end ) < 1 )
  {
    return false;
  }

  std::vector<Type> vec_val( begin, end );
  std::sort( vec_val.begin(), vec_val.end() );
  min = vec_val[0];
  max = vec_val[vec_val.size() - 1];
  mean = accumulate( vec_val.begin(), vec_val.end(), Type( 0 ) )
         / static_cast<Type>( vec_val.size() );
  median = vec_val[vec_val.size() / 2];
  return true;
}


/**
* @brief Display to standard output min, max, mean and median value of input range
* @param begin start of range
* @param end end of range
*/
template <typename Type, typename DataInputIterator>
void minMaxMeanMedian( DataInputIterator begin, DataInputIterator end )
{
  Type min, max, mean, median;
  minMaxMeanMedian( begin, end, min, max, mean, median );
  std::cout << "\n"
            << "\t min: " << min << "\n"
            << "\t mean: " << mean << "\n"
            << "\t median: " << median << std::endl
            << "\t max: " << max << std::endl;
}

/**
 ** Split a range [ a ; b [ into a set of n ranges :
 [ a ; c1 [ U [ c1 ; c2 [ U ... U [ c(n-1) ; b [
  **
  Output range vector only store [ a , c1 , c2 , ... , b ]

 ** if input range can't be split (range [a;b[ size is less than nb_split, only return [a;b[ range
 **
 ** @param range_start Start of range to split
 ** @param range_end End of range to split
 ** @param nb_split Number of desired split
 ** @param d_range Output splitted range
 **/
template < typename T >
void SplitRange( const T range_start , const T range_end , const int nb_split ,
                 std::vector< T > & d_range )
{
  const T range_length = range_end - range_start ;
  if( range_length < nb_split )
  {
    d_range.push_back( range_start ) ;
    d_range.push_back( range_end ) ;
  }
  else
  {
    const T delta_range = range_length / nb_split ;

    d_range.push_back( range_start ) ;
    for( int i = 1 ; i < nb_split ; ++i )
    {
      d_range.push_back( range_start + i * delta_range ) ;
    }
    d_range.push_back( range_end ) ;
  }
}


} // namespace open3DCV


#endif  // OPENMVG_NUMERIC_NUMERIC_H
