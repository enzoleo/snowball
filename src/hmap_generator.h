/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _HMAP_GENERATOR_H_
#define _HMAP_GENERATOR_H_

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map>
#include <print>
#include <string>

#define _ALLOCATION_FAILED_ 1
#define _VECTOR_ILLEGAL_SIZE_ 2

/* The data type of image pointer and noise */
typedef GLfloat** ImagePointer;
typedef GLfloat** NoiseType;

/* The algorithms to generate height map */
enum GeneratorAlgorithm {
  NONE_ALGORITHM,
  PERLIN_NOISE_ALGORITHM,
  CIRCLE_STRIKE_ALGORITHM
};

/* Definite parameters type */
typedef std::map<std::string, void*> MapParams;

/* STRUCT: parameters (base) */
struct HeightMapParams : public MapParams {
  /* Iterations in STL map */
  typedef typename MapParams::iterator _iter;

  /* Default constructor & Constructor */
  HeightMapParams(GeneratorAlgorithm _algorithm = NONE_ALGORITHM,
                  const std::string& _heightMapPath = "../assets/terrains/",
                  GLuint _width = 256,
                  GLuint _height = 256) {
    // Malloc space and assignment
    std::string* _path_ptr = new std::string;
    GeneratorAlgorithm* _agrtm_ptr = new GeneratorAlgorithm;
    GLuint* _width_ptr = new GLuint;
    GLuint* _height_ptr = new GLuint;

    *_path_ptr = _heightMapPath;
    *_agrtm_ptr = _algorithm;
    *_width_ptr = _width;
    *_height_ptr = _height;

    // Insert pair to this map
    (*this)["algorithm"] = reinterpret_cast<void*>(_agrtm_ptr);
    (*this)["heightMapPath"] = reinterpret_cast<void*>(_path_ptr);
    (*this)["width"] = reinterpret_cast<void*>(_width_ptr);
    (*this)["height"] = reinterpret_cast<void*>(_height_ptr);
  }

  /* Functions to get value with a key */
  template <class T>
  T get_param(const std::string& param_name, const T& default_value) {
    // Find key in STL map
    _iter it = (*this).find(param_name);
    if (it != (*this).end())
      return *reinterpret_cast<T*>(it->second);

    // Return default value if the key does not exist
    return default_value;
  }

  template <typename T>
  T get_param(const std::string& param_name) {
    _iter it = (*this).find(param_name);
    if (it != (*this).end())
      return *reinterpret_cast<T*>(it->second);
    else {  // Report problem and throw exceptions if the key does not exist
      std::print(stderr, "ERROR: Missing parameter {}.\n", param_name);
      throw param_name;
    }
  }
};

/* CLASS: Height map generator
** The class generates a height map and save it. */
class HeightMapGeneratorBase {
 public:
  /* Default constructor & Constructor */
  HeightMapGeneratorBase(const std::string& _heightMapPath = "../assets/terrains/",
                         GLuint _width = 256,
                         GLuint _height = 256)
      : heightMapPath(_heightMapPath),
        algorithm(NONE_ALGORITHM),
        width(_width),
        height(_height) {  // Do nothing here
  }

  /* Constructor with specific parameters */
  HeightMapGeneratorBase(const HeightMapParams& params) {  // Find value with given key
    HeightMapParams params_copy = params;
    heightMapPath = params_copy.get_param<std::string>("heightMapPath", "../assets/terrains/");
    width = params_copy.get_param<GLuint>("width", 256);
    height = params_copy.get_param<GLuint>("height", 256);
    algorithm = params_copy.get_param<GeneratorAlgorithm>("algorithm", NONE_ALGORITHM);
  }

  /* Returns the private members
  ** We set the heightMap generator settings private, because they are not supposed to be
  ** editted easily. If they need to be editted, call the `set*` functions, which makes
  ** sure you edit them on purpose, instead of unconsciously. */
  const std::string& getPath() { return heightMapPath; }
  const GeneratorAlgorithm getAlgorithm() { return algorithm; }
  const GLuint getWidth() { return width; }
  const GLuint getHeight() { return height; }

  /* Reset some private members */
  void setPath(const std::string& path) { heightMapPath = path; }

  /* Virtual functions */
  virtual void generate(const char* filename) = 0;

 protected:
  /* FUNCTION: save height map
  ** DEFAULT directory: "../assets/terrains/"
  ** The height map will be saved into this directory */
  void save(ImagePointer heightMapData,
            const std::string& filename) {
    // Get the absolute path of the height map file
    std::string ap_filename = heightMapPath + filename;
    unsigned char* heightMapPixels;

    // ----------------------------------------------------------------------------
    // DECLARE: a height map
    // Why use `unsigned char`?
    // Notice that data with unsigned char type is allowed to be valued from ASCII
    // 0 to 255. Therefore, We store the height data as unsigned char datatype in
    // pixel RGB. For example, a pixel has height data H, then we set the RGB value
    // of this pixel (H, H, H).
    // ----------------------------------------------------------------------------
    try {  // Initialization (assignment)!
      heightMapPixels = new unsigned char[width * height];
      for (GLuint hloop = 0; hloop < height; hloop++) {
        for (GLuint wloop = 0; wloop < width; wloop++) {  // Convert the data into unsigned char datatype
          heightMapPixels[hloop * width + wloop] = (unsigned char)(heightMapData[hloop][wloop] * 255);
        }
      }
    } catch (const std::bad_alloc& err) {  // Catch the allocation error
      std::print(stderr, "ERROR: Allocation failed!\n");
      exit(1);
    }

    // Create a SDL surface by computed pointer.
    // Attention the depth is 8bits here, so the SDL function `SDL_CreateRGBSurfaceFrom`
    // allocates an empty palette for the surface. We have to fill the palette by hand.
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        heightMapPixels, width, height, 8,
        width, 0, 0, 0, 0);

    // Initiate a new grayscale palette
    // Learn more info here: https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
    SDL_Color colors[256];
    for (int i = 0; i < 256; i++)
      colors[i].r = colors[i].g = colors[i].b = i;
    SDL_SetPaletteColors(surface->format->palette, colors, 0, 256);

    // Save height map image
    SDL_SaveBMP(surface, ap_filename.c_str());
    SDL_FreeSurface(surface);

    // Delete the height_map
    delete[] heightMapPixels;
  }

  /* PROTECTED MEMBER
  ** The directory of height map.
  ** Default: "../assets/terrains/" */
  std::string heightMapPath;

  /* PRIVATE MEMBER
  ** The algorithm used */
  GeneratorAlgorithm algorithm;

  /* PRIVATE MEMBERS
  ** The width and height of the height map */
  GLuint width, height;
};

/* STRUCT: parameters (circle strike algorithms) */
struct CircleStrikeParams : public HeightMapParams {
  /* Default constructor & Constructor */
  CircleStrikeParams(GLfloat _circleRadius = 50.0f,
                     GLfloat _forwardFlx = 1.0f,
                     GLfloat _backwardFlx = -1.0f,
                     const std::string& _heightMapPath = "../assets/terrains/")
      : HeightMapParams(CIRCLE_STRIKE_ALGORITHM, _heightMapPath) {
    // New several pointers
    GLfloat* radius_ptr = new GLfloat;
    GLfloat* for_flx_ptr = new GLfloat;
    GLfloat* back_flx_ptr = new GLfloat;

    // Do assignments of each variable
    *radius_ptr = _circleRadius;
    *for_flx_ptr = _forwardFlx;
    *back_flx_ptr = _backwardFlx;

    // Insert to STL map
    (*this)["circleRadius"] = reinterpret_cast<void*>(radius_ptr);
    (*this)["forwardFlx"] = reinterpret_cast<void*>(for_flx_ptr);
    (*this)["backwardFlx"] = reinterpret_cast<void*>(back_flx_ptr);
  }
};

/* CLASS: Height map generator
** Using circle strike algorithm */
class CircleStrikeGenerator : public HeightMapGeneratorBase {
 public:
  /* Default constructor & Constructor */
  CircleStrikeGenerator(GLfloat _circleRadius = 50.0f,
                        GLfloat _forwardFlx = 1.0f,
                        GLfloat _backwardFlx = -1.0f,
                        GLuint _iterations = 100,
                        GLuint _stableIterNum = 100,
                        GLfloat _maxCrash = 1.0f,
                        GLfloat _minCrash = 0.1f,
                        const std::string& _heightMapPath = "../assets/terrains/")
      : HeightMapGeneratorBase(_heightMapPath),
        circleRadius(_circleRadius),
        forwardFlx(_forwardFlx),
        backwardFlx(_backwardFlx),
        iterations(_iterations),
        stableIterNum(_stableIterNum),
        maxCrash(_maxCrash),
        minCrash(_minCrash) {  // Do nothing here
    algorithm = CIRCLE_STRIKE_ALGORITHM;
  }

  /* Constructor with specific parameters */
  CircleStrikeGenerator(const CircleStrikeParams& params,
                        GLuint _iterations = 100,
                        GLuint _stableIterNum = 100,
                        GLfloat _maxCrash = 1.0f,
                        GLfloat _minCrash = 0.1f)
      : HeightMapGeneratorBase(params),
        iterations(_iterations),
        stableIterNum(_stableIterNum),
        maxCrash(_maxCrash),
        minCrash(_minCrash) {  // Find value with given key
    CircleStrikeParams params_copy = params;
    circleRadius = params_copy.get_param<GLfloat>("circleRadius", 50.0f);
    forwardFlx = params_copy.get_param<GLfloat>("forwardFlx", 1.0f);
    backwardFlx = params_copy.get_param<GLfloat>("backwardFlx", -1.0f);
  }

  /* Returns the private members
  ** We set the heightMap generator settings private, because they are not supposed to be
  ** editted easily. If they need to be editted, call the `set*` functions, which makes
  ** sure you edit them on purpose, instead of unconsciously. */
  const GLfloat getCircleRadius() { return circleRadius; }
  const GLfloat getForwardFlx() { return forwardFlx; }
  const GLfloat getBackwardFlx() { return backwardFlx; }
  const GLuint getIterations() { return iterations; }
  const GLuint getStableIterNum() { return stableIterNum; }
  const GLfloat getMaxCrash() { return maxCrash; }
  const GLfloat getMinCrash() { return minCrash; }

  /* Reset some private members */
  void setCircleRadius(const GLfloat _circleRadius) { circleRadius = _circleRadius; }
  void setForwardFlx(const GLfloat _forwardFlx) { forwardFlx = _forwardFlx; }
  void setBackwardFlx(const GLfloat _backwardFlx) { backwardFlx = _backwardFlx; }
  void setIterations(const GLuint _iterations) { iterations = _iterations; }
  void setStableIterNum(const GLuint _stableIterNum) { stableIterNum = _stableIterNum; }
  void setMaxCrash(const GLfloat _maxCrash) { maxCrash = _maxCrash; }
  void setMinCrash(const GLfloat _minCrash) { minCrash = _minCrash; }

  /******************************************
  ** GENERATE function:
  **     The function generates a height map and save it to default directory.
  **
  ** @param filename: The name of the height map.
  ******************************************/
  void generate(const char* filename) {
    // Set seed for random number generator
    srand(time(0));
    // Record the iterations algorithm has done
    GLuint iter_done = 0;
    // The height data of the height map
    GLfloat** heightMapData;

    try {  // Initialization (assignment)!
      heightMapData = new GLfloat*[width];
      for (NoiseType row_ptr = heightMapData;
           row_ptr != heightMapData + width;
           row_ptr++) {
        *row_ptr = new GLfloat[height];
        for (GLfloat* col_ptr = *row_ptr;
             col_ptr != *row_ptr + height;
             col_ptr++)
          *col_ptr = 0.0f;
      }
    } catch (const std::bad_alloc& err) {  // Catch the allocation error
      std::print(stderr, "ERROR: Allocation failed!\n");
      exit(1);
    }

    // Parameter @manValue records the maximum or minimum height
    GLfloat maxValue = 0.0f;
    for (GLuint iter = 0; iter < iterations; iter++) {
      // Get the center coordinates of circle randomly
      GLfloat z = ((GLfloat)rand() / RAND_MAX) * width;
      GLfloat x = ((GLfloat)rand() / RAND_MAX) * height;

      // The impact of strike will decrease with iterations increasing
      iter_done++;
      GLfloat disp = minCrash;
      if (iter_done < stableIterNum)
        disp = minCrash + (iter_done / stableIterNum) * (maxCrash - minCrash);

      // ----------------------------------------------------------------------------
      // We want to choose a number from a finite set {-1, 1} randomly. The following
      // method is available. This is a really simple method: choose a float number
      // @rand_num randomly from (0, 1), then the probability P(@rand_num > 0.5) act-
      // ually equals to the probability P(@rand_bum <= 0.5).
      // ----------------------------------------------------------------------------
      GLfloat rand_num = (GLfloat)rand() / RAND_MAX;
      // Determines the strike direction (forward or backward) randomly
      GLboolean strike_direction_flag = false;
      if (rand_num > 0.5f) strike_direction_flag = true;

      for (GLuint hloop = 0; hloop < height; hloop++) {
        for (GLuint wloop = 0; wloop < width; wloop++) {
          // Declare distance variables
          GLfloat dist_ratio = 0.0f;
          GLfloat crashAux = 0.0f;
          // Compute the distance between the circle center and grid points
          dist_ratio = sqrt((hloop - x) * (hloop - x) + (wloop - z) * (wloop - z)) / circleRadius;

          // If one grid point is outside the circle, it will not be effected
          // Or, the circle strike will change the height of this point
          if (dist_ratio > 1)
            crashAux = 0.0f;
          else {
            if (strike_direction_flag)  // Forward strike
              crashAux = (1 + cos(dist_ratio * M_PI)) * disp / 2 * forwardFlx;
            else  // Backward strike
              crashAux = -(1 + cos(dist_ratio * M_PI)) * disp / 2 * backwardFlx;
          }
          GLfloat heightTemp = heightMapData[hloop][wloop] + crashAux;
          heightMapData[hloop][wloop] = heightTemp;

          // Compute the maximum height of minimum height
          if (heightTemp > maxValue || heightTemp < -maxValue)
            maxValue = fabs(heightTemp);
        }
      }
    }

    // This step is quite important!
    // We can only store data in [0, 1) into an image
    for (GLuint hloop = 0; hloop < height; hloop++) {
      for (GLuint wloop = 0; wloop < width; wloop++) {
        heightMapData[hloop][wloop] /= maxValue;
      }
    }

    // FINAL step: save the height map
    save(heightMapData, std::string(filename));
  }

 protected:
  /* PROTECTED MEMBER
  ** The radius of the circles */
  GLfloat circleRadius;

  /* PROTECTED MEMBERS
  ** Determines the FLEXIBILITY of the plane when it is striked by a
  ** circle from forward or backward.
  ** NOTE: the value of them is supposed to be [0, +infty) */
  GLfloat forwardFlx, backwardFlx;

  /* PROTECTED MEMBER
  ** The iterations of operating steps the algorithm will do. */
  GLuint iterations;

  /* PROTECTED MEMBER
  ** The effection factor will decrease until @stableIterNum iterations
  ** the algorithm has done. */
  GLuint stableIterNum;

  /* PROTECTED MEMBERS
  ** @param maxCrash: The maximum crash (when iterations begin).
  ** @param minCrash: The minimum crash (when iterations end). */
  GLfloat maxCrash, minCrash;
};

/* STRUCT: parameters (Perlin noise algorithms) */
struct PerlinNoiseParams : public HeightMapParams {
  /* Default constructor & Constructor */
  PerlinNoiseParams(GLfloat _smooth = 2.0f,
                    GLuint _persistence = 2,
                    const std::string& _heightMapPath = "../assets/terrains/")
      : HeightMapParams(PERLIN_NOISE_ALGORITHM, _heightMapPath) {
    // New several pointers and do assignments
    GLfloat* smth_ptr = new GLfloat;
    GLuint* pers_ptr = new GLuint;
    *smth_ptr = _smooth;
    *pers_ptr = _persistence;

    // Insert to STL map
    (*this)["smooth"] = reinterpret_cast<void*>(smth_ptr);
    (*this)["persistence"] = reinterpret_cast<void*>(pers_ptr);
  }
};

/* CLASS: Height map generator
** Using perlin noise algotithm. */
class PerlinNoiseGenerator : public HeightMapGeneratorBase {
 public:
  /* Default constructor & Constructor */
  PerlinNoiseGenerator(GLfloat _smooth = 2.0f,
                       GLuint _persistence = 2,
                       GLuint octaves = 8,
                       const std::string& _heightMapPath = "../assets/terrains/")
      : HeightMapGeneratorBase(_heightMapPath),
        smooth(_smooth),
        persistence(_persistence),
        octaves(8) {  // Do nothing here
    algorithm = PERLIN_NOISE_ALGORITHM;
  }

  /* Constructor with specific parameters */
  PerlinNoiseGenerator(const PerlinNoiseParams& params,
                       GLuint octaves = 8)
      : HeightMapGeneratorBase(params),
        octaves(8) {  // Find value with given key
    PerlinNoiseParams params_copy = params;
    smooth = params_copy.get_param<GLfloat>("smooth", 2.0f);
    persistence = params_copy.get_param<GLuint>("persistence", 2);
  }

  /* Returns the private members
  ** We set the heightMap generator settings private, because they are not supposed to be
  ** editted easily. If they need to be editted, call the `set*` functions, which makes
  ** sure you edit them on purpose, instead of unconsciously. */
  const GLfloat getSmooth() { return smooth; }
  const GLuint getPersistence() { return persistence; }
  const GLuint getOctaves() { return octaves; }

  /* Reset some private members */
  void setSmooth(const GLfloat _smooth) { smooth = _smooth; }
  void setPersistence(const GLuint _persistence) { persistence = _persistence; }
  void setOctaves(const GLuint _octaves) { octaves = _octaves; }

  /******************************************
  ** GENERATE function:
  **     The function generates a height map and save it to default directory.
  **
  ** @param filename: The name of the height map.
  ******************************************/
  void generate(const char* filename) {
    // Generate white noise
    NoiseType whiteNoise = generateWhiteNoise();
    // Declare smooth noise and perlin noise
    NoiseType* smoothNoises;
    NoiseType perlinNoise;

    // ----------------------------------------------------------------------------
    // Throw exceptions and catch it!
    // Notice the differences between `new-delete` and `malloc-free`.
    // If `new` qualifier failed to do allocation, it will throw an exception. Then
    // we catch the exception and exit the program.
    // ----------------------------------------------------------------------------
    try {  // Initialization!
      smoothNoises = new NoiseType[octaves];
      GLint octave_num = 0;
      for (NoiseType* noise_ptr = smoothNoises;
           noise_ptr != smoothNoises + octaves;
           noise_ptr++, octave_num++) {
        *noise_ptr = generateSmoothNoise(whiteNoise, octave_num);
      }

      // Initialise the perlin noise matrix
      // Do traversal and value each element
      perlinNoise = new GLfloat*[width];
      for (NoiseType row_ptr = perlinNoise;
           row_ptr != perlinNoise + width;
           row_ptr++) {
        *row_ptr = new GLfloat[height];
        for (GLfloat* col_ptr = *row_ptr;
             col_ptr != *row_ptr + height;
             col_ptr++)
          *col_ptr = 0.0f;
      }
    } catch (const std::bad_alloc& err) {  // Catch the allocation error
      std::print(stderr, "ERROR: Allocation failed!\n");
      exit(1);
    }

    // The standard amplitude and the sum of all aplitudes
    GLfloat amplitude = 1.0f;
    GLfloat ampSum = amplitude * (1 - pow(1.0f / smooth, octaves)) / (smooth - 1);

    // Take attention how the algorithm works
    for (NoiseType* noise_ptr = smoothNoises + octaves;
         noise_ptr != smoothNoises;
         noise_ptr--) {
      // Decrease the amplitude!
      amplitude /= smooth;

      // A flag determines whether the outer loop comes to end
      GLboolean end_flag = false;
      if (noise_ptr == smoothNoises + 1) end_flag = true;

      for (GLuint hloop = 0; hloop < height; hloop++) {  // Do traversal for each pixel
        for (GLuint wloop = 0; wloop < width; wloop++) {
          perlinNoise[hloop][wloop] += (*(noise_ptr - 1))[hloop][wloop] * amplitude;
          // Average the perlin noise data
          if (end_flag) perlinNoise[hloop][wloop] /= ampSum;
        }
      }
    }

    // FINAL step: save the height map
    save(perlinNoise, std::string(filename));
  }

 protected:
  /******************************************
  ** GENERATE FUNCTION:
  **     The function generates white noise.
  **
  ** @param width: Of course the width of the height map (pixels)
  ** @param height: Of course the height of the height map (pixels)
  ** @param seed: The seed of random number generator.
  **
  ** Actually this function generates a random matrix with width * height size
  ******************************************/
  NoiseType generateWhiteNoise() {
    // Set seed of random number generator
    srand(time(0));
    NoiseType whiteNoise;

    try {  // Initialization (assignment)!
      whiteNoise = new GLfloat*[width];
      for (NoiseType row_ptr = whiteNoise;
           row_ptr != whiteNoise + width;
           row_ptr++) {
        *row_ptr = new GLfloat[height];
        for (GLfloat* col_ptr = *row_ptr;
             col_ptr != *row_ptr + height;
             col_ptr++)
          *col_ptr = (GLfloat)rand() / RAND_MAX;
      }
    } catch (const std::bad_alloc& err) {  // Catch the allocation error
      std::print(stderr, "ERROR: Allocation failed!\n");
      exit(1);
    }

    return whiteNoise;
  }

  /******************************************
  ** GENERATE FUNCTION:
  **     The function generates smooth noise.
  **
  ** @param whiteNoise: The white noise matrix to be modified.
  ** @param width: Of course the width of the height map (pixels)
  ** @param height: Of course the height of the height map (pixels)
  ** @param octaves: A parameter about frequency.
  **
  ** Actually this function generates a random matrix with width * height size
  ******************************************/
  NoiseType generateSmoothNoise(NoiseType whiteNoise,
                                GLuint octave) {
    // Calculate the period and frequency
    GLint period = pow(persistence, octave);
    GLfloat frequency = 1.0f / period;

    // Declare smooth noise
    NoiseType smoothNoise;
    try {  // Initialization (assignment)!
      smoothNoise = new GLfloat*[width];
      for (GLuint hloop = 0; hloop < height; hloop++) {
        smoothNoise[hloop] = new GLfloat[height];

        // -------------------------------------------------------------------------
        // Take attention how the algorithm works!
        //
        // @cur_wperiod determines the number of periods the width loop passes.
        // @next_wperiod determines the point the next width period ends at.
        // @cur_hperiod determines the number of periods the height loop passes.
        // @next_hperiod determines the point the next height period ends at.
        //
        // The value of parameter @period is quite important in this algorithm! It
        // is determined by the private member @persistence and outer parameter
        // @octave. The @whiteNoise matrix will be divided by square with period *
        // period size. Inside each square, the value will be editted by the follow-
        // ing algorithm, which makes the hight map smooth.
        // -------------------------------------------------------------------------
        GLint cur_wperiod = (hloop / period) * period;
        GLint next_wperiod = (cur_wperiod + period) % width;

        // Do interpolation and assignment
        GLfloat horizontal_blend = (hloop - cur_wperiod) * frequency;
        for (GLuint wloop = 0; wloop < width; wloop++) {
          GLint cur_hperiod = (wloop / period) * period;
          GLint next_hperiod = (cur_hperiod + period) % height;
          GLfloat vertical_blend = (wloop - cur_hperiod) * frequency;

          // Do interpolation inside each square
          GLfloat top = interpolate(whiteNoise[cur_wperiod][cur_hperiod],
                                    whiteNoise[next_wperiod][cur_hperiod],
                                    horizontal_blend);
          GLfloat bottom = interpolate(whiteNoise[cur_wperiod][next_hperiod],
                                       whiteNoise[next_wperiod][next_hperiod],
                                       horizontal_blend);
          smoothNoise[hloop][wloop] = interpolate(top, bottom, vertical_blend);
        }
      }
    } catch (const std::bad_alloc& err) {  // Catch the allocation error
      std::print(stderr, "ERROR: Allocation failed!\n");
      exit(1);
    }

    return smoothNoise;
  }

  /* INTERPOLATION */
  GLfloat interpolate(GLfloat top, GLfloat bottom, GLfloat factor) {
    return (top + bottom) / 2.0f + (top - bottom) * cos(factor * M_PI) / 2.0f;
  }

  /* PROTECTED MEMBER
  ** The smooth variable of the height map generator
  ** The bigger this value is, the smoother height map will be. */
  GLfloat smooth;

  /* PROTECTED MEMBER
  ** The persistence coefficients
  ** Used in smooth noise generator */
  GLuint persistence;

  /* PROTECTED MEMBER
  ** @param octaves: A parameter about frequency. The value of this parameter @octave must
  **     be an positive integer with 2^n form, which means the actual frequency (f) is 2^n
  **     times the reference frequency (f0). This is an electronical concept. To learn more
  **     about this, take a look at `https://en.wikipedia.org/wiki/Octave_(electronics)`. */
  GLuint octaves;
};

/* CLASS: All algorithms */
class HeightMapGenerator {
 public:
  /* Default constructor & Constructor
  ** Using parameters of Circle Strike Algorithm */
  HeightMapGenerator(const CircleStrikeParams& params,
                     GLuint _iterations = 100,
                     GLuint _stableIterNum = 100,
                     GLfloat _maxCrash = 1.0f,
                     GLfloat _minCrash = 0.1f) {
    _base_generator = new CircleStrikeGenerator(
        params, _iterations, _stableIterNum, _maxCrash, _minCrash);
    // Copy the parameters
    _generator_params = params;
  }

  /* Default constructor & Constructor
  ** Using parameters of Perlin Noise Algorithm */
  HeightMapGenerator(const PerlinNoiseParams& params,
                     GLuint octaves = 8) {
    _base_generator = new PerlinNoiseGenerator(params, octaves);
    // Copy the parameters
    _generator_params = params;
  }

  /* Function to return private member: @_generator_params */
  HeightMapParams getParams() { return _generator_params; }

  /* Generate height map and save it */
  void generate(const char* filename) {
    _base_generator->generate(filename);
  }

 private:
  /* IMPORTANT PRIVATE MEMBER
  ** This pointer saves the address of one specific generator
  ** using algorithm in @_generator_params */
  HeightMapGeneratorBase* _base_generator;

  /* PRIVATE MEMBER
  ** The parameters including specific algorithm */
  HeightMapParams _generator_params;
};

#endif
