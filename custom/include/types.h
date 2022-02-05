#ifndef CUSTOM_TYPES_H_
#define CUSTOM_TYPES_H_

// Put here your custom data types shared by many source files.

// NB: Each color percentage is in interval [0, 10000]
typedef struct {
  int32_t red;
  int32_t green;
  int32_t blue;
} RGBPercentage;

typedef struct {
  float x, y;
} Point;

#endif /* CUSTOM_TYPES_H_ */
