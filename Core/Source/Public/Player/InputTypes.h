// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

typedef uint8 LKey;

namespace EKeys
{

String ToString(const LKey& Key);

inline constexpr LKey KeyBegin                  {   0 };
inline constexpr LKey KeyEnd                    { 255 };
/* Statically assert this so that in the future we do not forget to change begin and end - when changing the type. */
static_assert(sizeof(LKey) == 1, "LKey must be 1 byte in size.");

inline constexpr LKey AnyKey                    {   0 };

inline constexpr LKey A                         {   1 };
inline constexpr LKey B                         {   2 };
inline constexpr LKey C                         {   3 };
inline constexpr LKey D                         {   4 };
inline constexpr LKey E                         {   5 };
inline constexpr LKey F                         {   6 };
inline constexpr LKey G                         {   7 };
inline constexpr LKey H                         {   8 };
inline constexpr LKey I                         {   9 };
inline constexpr LKey J                         {  10 };
inline constexpr LKey K                         {  11 };
inline constexpr LKey L                         {  12 };
inline constexpr LKey M                         {  13 };
inline constexpr LKey N                         {  14 };
inline constexpr LKey O                         {  15 };
inline constexpr LKey P                         {  16 };
inline constexpr LKey Q                         {  17 };
inline constexpr LKey R                         {  18 };
inline constexpr LKey S                         {  19 };
inline constexpr LKey T                         {  20 };
inline constexpr LKey U                         {  21 };
inline constexpr LKey V                         {  22 };
inline constexpr LKey W                         {  23 };
inline constexpr LKey X                         {  24 };
inline constexpr LKey Y                         {  25 };
inline constexpr LKey Z                         {  26 };

inline constexpr LKey MouseX                    {  27 };
inline constexpr LKey MouseY                    {  28 };
inline constexpr LKey MouseXY                   {  29 };
inline constexpr LKey MouseWheelAxis            {  30 };
inline constexpr LKey MouseWheelUp              {  31 };
inline constexpr LKey MouseWheelDown            {  32 };

inline constexpr LKey LeftMouseButton           {  33 };
inline constexpr LKey RightMouseButton          {  34 };
inline constexpr LKey MiddleMouseButton         {  35 };
inline constexpr LKey ThumbMouseButton1         {  36 };
inline constexpr LKey ThumbMouseButton2         {  37 };

inline constexpr LKey BackSpace                 {  38 };
inline constexpr LKey Tab                       {  39 };
inline constexpr LKey Enter                     {  40 };
inline constexpr LKey Pause                     {  41 };
inline constexpr LKey CapsLock                  {  42 };
inline constexpr LKey Escape                    {  43 };
inline constexpr LKey Space                     {  44 };
inline constexpr LKey PageUp                    {  45 };
inline constexpr LKey PageDown                  {  46 };
inline constexpr LKey End                       {  47 };
inline constexpr LKey Home                      {  48 };

inline constexpr LKey Left                      {  49 };
inline constexpr LKey Up                        {  50 };
inline constexpr LKey Right                     {  51 };
inline constexpr LKey Down                      {  52 };

inline constexpr LKey Insert                    {  53 };
inline constexpr LKey Delete                    {  54 };

inline constexpr LKey Zero                      {  55 };
inline constexpr LKey One                       {  56 };
inline constexpr LKey Two                       {  57 };
inline constexpr LKey Three                     {  58 };
inline constexpr LKey Four                      {  59 };
inline constexpr LKey Five                      {  60 };
inline constexpr LKey Six                       {  61 };
inline constexpr LKey Seven                     {  62 };
inline constexpr LKey Eight                     {  63 };
inline constexpr LKey Nine                      {  64 };

inline constexpr LKey NumPadZero                {  65 };
inline constexpr LKey NumPadOne                 {  66 };
inline constexpr LKey NumPadTwo                 {  67 };
inline constexpr LKey NumPadThree               {  68 };
inline constexpr LKey NumPadFour                {  69 };
inline constexpr LKey NumPadFive                {  70 };
inline constexpr LKey NumPadSix                 {  71 };
inline constexpr LKey NumPadSeven               {  72 };
inline constexpr LKey NumPadEight               {  73 };
inline constexpr LKey NumPadNine                {  74 };
inline constexpr LKey NumPadLock                {  75 };
inline constexpr LKey NumPadDivide              {  76 };
inline constexpr LKey NumPadMultiply            {  77 };
inline constexpr LKey NumPadSubtract            {  78 };
inline constexpr LKey NumPadAdd                 {  79 };
inline constexpr LKey NumPadEnter               {  80 };
inline constexpr LKey NumPadDecimal             {  81 };

inline constexpr LKey F1                        {  82 };
inline constexpr LKey F2                        {  83 };
inline constexpr LKey F3                        {  84 };
inline constexpr LKey F4                        {  85 };
inline constexpr LKey F5                        {  86 };
inline constexpr LKey F6                        {  87 };
inline constexpr LKey F7                        {  88 };
inline constexpr LKey F8                        {  89 };
inline constexpr LKey F9                        {  90 };
inline constexpr LKey F10                       {  91 };
inline constexpr LKey F11                       {  92 };
inline constexpr LKey F12                       {  93 };

inline constexpr LKey ScrollLock                {  94 };
inline constexpr LKey Print                     {  95 };

inline constexpr LKey LeftShift                 {  96 };
inline constexpr LKey RightShift                {  97 };
inline constexpr LKey LeftControl               {  98 };
inline constexpr LKey RightControl              {  99 };
inline constexpr LKey LeftAlt                   { 100 };
inline constexpr LKey RightAlt                  { 101 };
inline constexpr LKey LeftCommand               { 102 };
inline constexpr LKey RightCommand              { 103 };

inline constexpr LKey Semicolon                 { 104 };
inline constexpr LKey Equals                    { 105 };
inline constexpr LKey Comma                     { 106 };
inline constexpr LKey Underscore                { 107 };
inline constexpr LKey Hyphen                    { 108 };
inline constexpr LKey Period                    { 109 };
inline constexpr LKey Slash                     { 110 };
inline constexpr LKey Tilde                     { 111 };
inline constexpr LKey LeftBracket               { 112 };
inline constexpr LKey Backslash                 { 113 };
inline constexpr LKey RightBracket              { 114 };
inline constexpr LKey Apostrophe                { 115 };

inline constexpr LKey Ampersand                 { 116 };
inline constexpr LKey Asterisk                  { 117 };
inline constexpr LKey Caret                     { 118 };
inline constexpr LKey Colon                     { 119 };
inline constexpr LKey Dollar                    { 120 };
inline constexpr LKey Exclamation               { 121 };
inline constexpr LKey LeftParenthesis           { 122 };
inline constexpr LKey RightParenthesis          { 123 };
inline constexpr LKey Quote                     { 124 };

/** Platform specific virtual delete key. */
inline constexpr LKey PlatformDelete            { EKeys::KeyEnd };

}
