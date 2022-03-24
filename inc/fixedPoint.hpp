#ifndef __FIXED_POINT_HPP__
#define __FIXED_POINT_HPP__

#define MAX_FIXED_POINT_LENGTH	64
#define DEFAULT_NUM_FRAC_BITS	32


#include <stdint.h>
#include <iostream>
#include <limits.h>
#include <cstdint> 
#include <math.h>
#include <signal.h>

#ifndef NDEBUG
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


//! typedef for fixedPoint variable
typedef int64_t fixedPoint_t;


class fixedPoint {

	public:   
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------
		fixedPoint();
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class
         *      \param[in]          value input to be converted to fixedPoint
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		fixedPoint(int64_t value);

        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class
         *      \param[in]          value input to be converted to fixedPoint
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		fixedPoint(float value);

        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class
         *      \param[in]          length fixedPoint value length
         *      \param[in]          numFracBits number of fractional bits for fixedPoint value 
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		fixedPoint(int length, int numFracBits);


        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class
         *      \param[in]          length Description of variable one, an input
         *      \param[in]          numFracBits Description of variable three, an output 
         *      \param[in]          value input to be converted to fixedPoint
         *      \param[in]          normalize flag to appropiately mask and shift integer and fractional bits         
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		fixedPoint(int length, int numFracBits, int64_t value, bool normalize = true);

        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    constructor for fixedPoint class; does normalization
         *      \param[in]          length Description of variable one, an input
         *      \param[in]          numFracBits Description of variable three, an output 
         *      \param[in]          value input to be converted to fixedPoint
         *      \return             pointer to fixedPoint object
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		fixedPoint(int length, int numFracBits, float value);


        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    creates a fixedPoint number; does normalization
         *      \param[in]          var1 Description of variable one, an input
         *      \param[out]         var3 Description of variable three, an output 
         *      \param[in,out]      var4 Description of variable four, an input/output (usu. via a pointer
         *      \return             returns a fixedPoint number
         *
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		static fixedPoint_t create(int length, int numFracBits, float value);
        

        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    destructor for fixedPoint Object
         *
		// --------------------------------------------------------------------------------------------------------------------------------------------------        
		~fixedPoint();


        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    extract fractional bits
         *      \return             fractional bits of fixedPoint value
         *                          
		 */
		// --------------------------------------------------------------------------------------------------------------------------------------------------
		int64_t GetFracPart();
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    extracted integer bits
         *      \param[in]          numFracBits number of fractional bits
         *      \param[in]          value input where fractional bits are extracted 
         *      \return             integer bits of fixedPoint value
		 *
         */
		// --------------------------------------------------------------------------------------------------------------------------------------------------
		static int64_t GetFracPart(int numFracBits, int64_t value);
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
         *	    \brief			    function description
         *      \return             integer bits of fixedPoint value
         *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		int64_t GetIntPart();

       
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		/**
		 *		\brief			    extracted integer bits
         *      \param[in]          numFracBits number of fractional bits
         *      \param[in]          value input where integer bits are extracted 
         *      \return             integer bits of fixedPoint value
		 *
         */
		// --------------------------------------------------------------------------------------------------------------------------------------------------
		static int64_t GetIntPart(int length, int numFracBits, int64_t value);
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
         *	    \brief			    retrieves value of fixedPoint object
         *      \return             value of fixedPoint object
         *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		int64_t GetValue();
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
         *	    \brief			    adjusts fixedPoint value to inputed length and fractional bits
         *      \param[in]          newLength new length used to adjust fixedPoint value
         *      \param[out]         newNumFracBits new number of fractional bits used to adjust fixedPoint value
         *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		void SetParam(int newLength, int newNumFracBits);
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
         *	    \brief			    adjusts fixedPoint value to inputed length and fractional bits
         *      \param[in]          oldLength old length used of fixedPoint value
         *      \param[in]          oldNumFracBits old number of fractional bits of fixedPoint value
         *      \param[in]          newLength new length used to adjust fixedPoint value
         *      \param[in]          newNumFracBits new number of fractional bits used to adjust fixedPoint value
         *      \param[in,out]      num fixedPoint value to adjust
         *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        static void SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t &num);
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
         *	    \brief			    adjusts fixedPoint value to inputed length and fractional bits
         *      \param[in]          oldLength old length used of fixedPoint value
         *      \param[in]          oldNumFracBits old number of fractional bits of fixedPoint value
         *      \param[in]          newLength new length used to adjust fixedPoint value
         *      \param[in]          newNumFracBits new number of fractional bits used to adjust fixedPoint value
         *      \param[in,out]      num_arry pointer to buffer containing fixedPoint values to adjust
         *      \param[in]          arryLength length of buffer holding fixedPoint values
         *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		static void SetParam(int oldLength, int oldNumFracBits, int newLength, int newNumFracBits, fixedPoint_t *num_arry, int arryLength);
        
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
        *	    \brief              converts fixedPoint value to floating point
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
		float toFloat();
		
        
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        /**
        *	    \brief              converts fixedPoint value to floating point
        *       \param[in]          numFracBits number of fractional bits for converted fixedPoint number
        *       \param[in]          value input to be converted to floating point
        *       \return             value converted to floating point
        *                           
        */
        // --------------------------------------------------------------------------------------------------------------------------------------------------
        static float toFloat(int numFracBits, fixedPoint_t value);
		
        
        //! operator overloaders, LHS - Left-hand-side; RHS - Right-Hand-Side
        friend fixedPoint operator+(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator-(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator*(fixedPoint &operand0, fixedPoint &operand1);
		friend fixedPoint operator/(fixedPoint &operand0, fixedPoint &operand1);
        fixedPoint& operator+=(fixedPoint &rhs);
        fixedPoint& operator-=(fixedPoint &rhs);
        fixedPoint& operator*=(fixedPoint &rhs);
        fixedPoint& operator/=(fixedPoint &rhs);
		fixedPoint& operator=(const fixedPoint &obj);
		friend bool operator<(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator<=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator>(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator>=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator==(const fixedPoint &operand0, const fixedPoint &operand1);
		friend bool operator!=(const fixedPoint &operand0, const fixedPoint &operand1);
		friend std::ostream& operator<<(std::ostream& o, fixedPoint value);


		int64_t		m_value;        /*!< holds fixedPoint value */ 
		int			m_numFracBits;  /*!< number of fractional bits in fixedPoint value */ 
		int			m_length;       /*!< total length of fixedPoint value */ 


	protected:


	private:


};

#endif