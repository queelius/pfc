/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "PFC - Prefix-Free Codecs", "index.html", [
    [ "Features", "index.html#autotoc_md1", null ],
    [ "Quick Start", "index.html#autotoc_md2", null ],
    [ "Building", "index.html#autotoc_md3", null ],
    [ "Test Coverage", "index.html#autotoc_md4", null ],
    [ "Library Structure", "index.html#autotoc_md5", [
      [ "Core Components (<tt>include/pfc/</tt>)", "index.html#autotoc_md6", null ],
      [ "Codecs Available", "index.html#autotoc_md7", [
        [ "Universal Integer Codes", "index.html#autotoc_md8", null ],
        [ "Numeric Types", "index.html#autotoc_md9", null ],
        [ "Composite Types", "index.html#autotoc_md10", null ]
      ] ]
    ] ],
    [ "Design Principles", "index.html#autotoc_md11", null ],
    [ "Advanced Usage", "index.html#autotoc_md12", [
      [ "Custom Codecs", "index.html#autotoc_md13", null ],
      [ "Type-Erased Containers", "index.html#autotoc_md14", null ],
      [ "Parallel Processing", "index.html#autotoc_md15", null ]
    ] ],
    [ "Performance", "index.html#autotoc_md16", [
      [ "Compression Comparison (10,000 geometric values)", "index.html#autotoc_md17", null ]
    ] ],
    [ "Documentation", "index.html#autotoc_md18", null ],
    [ "Contributing", "index.html#autotoc_md19", null ],
    [ "License", "index.html#autotoc_md20", null ],
    [ "Production Status", "index.html#autotoc_md21", [
      [ "Production-Ready Components", "index.html#autotoc_md22", null ],
      [ "Experimental/Limited Components", "index.html#autotoc_md23", null ]
    ] ],
    [ "Known Limitations", "index.html#autotoc_md24", null ],
    [ "Acknowledgments", "index.html#autotoc_md25", null ],
    [ "New Codecs Implementation Summary", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html", [
      [ "Overview", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md27", null ],
      [ "Implementation Details", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md28", [
        [ "Files Modified", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md29", null ],
        [ "Files Created", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md30", null ],
        [ "Files Updated", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md31", null ]
      ] ],
      [ "Test Results", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md32", [
        [ "All New Codec Tests Pass", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md33", null ],
        [ "Test Coverage", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md34", null ]
      ] ],
      [ "Codec Characteristics", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md35", [
        [ "VByte (Variable Byte)", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md36", null ],
        [ "Exponential-Golomb", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md37", null ],
        [ "Elias Omega", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md38", null ]
      ] ],
      [ "Performance Comparison", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md39", [
        [ "Real-World Data (Geometric Distribution)", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md40", null ],
        [ "Large Values", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md41", null ]
      ] ],
      [ "Design Philosophy Adherence", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md42", [
        [ "1. Simplicity", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md43", null ],
        [ "2. Composability", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md44", null ],
        [ "3. Orthogonality", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md45", null ],
        [ "4. Concepts", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md46", null ]
      ] ],
      [ "Succinct Structures Design", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md47", [
        [ "Foundation: SuccinctBitVector", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md48", null ],
        [ "Future Structures", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md49", null ]
      ] ],
      [ "Integration with PFC Ecosystem", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md50", [
        [ "Namespace Organization", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md51", null ],
        [ "Existing Integration", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md52", null ]
      ] ],
      [ "Usage Examples", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md53", [
        [ "Basic Encoding/Decoding", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md54", null ],
        [ "Codec Selection", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md55", null ],
        [ "Signed Values", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md56", null ]
      ] ],
      [ "Documentation", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md57", [
        [ "Comprehensive Documentation Provided", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md58", null ],
        [ "Key Documentation Features", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md59", null ]
      ] ],
      [ "Roadmap", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md60", [
        [ "Phase 1: Quick Wins ✓ COMPLETED", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md61", null ],
        [ "Phase 2: Foundation (In Progress)", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md62", null ],
        [ "Phase 3: Core Structures (Next)", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md63", null ],
        [ "Phase 4: Advanced Features", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md64", null ]
      ] ],
      [ "Quality Metrics", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md65", [
        [ "Test Coverage", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md66", null ],
        [ "Code Quality", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md67", null ],
        [ "Performance", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md68", null ]
      ] ],
      [ "Competitive Advantages", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md69", [
        [ "1. Zero-Copy Architecture", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md70", null ],
        [ "2. Composability", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md71", null ],
        [ "3. Type Safety", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md72", null ],
        [ "4. STL-Adjacent", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md73", null ]
      ] ],
      [ "Known Issues", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md74", null ],
      [ "Conclusion", "md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md75", null ]
    ] ],
    [ "Succinct Data Structures Design Document", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html", [
      [ "Executive Summary", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md78", null ],
      [ "Quick Wins - Phase 1 (COMPLETED)", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md79", [
        [ "Three New Universal Codes", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md80", [
          [ "1. VByte (Variable Byte / Varint) ✓", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md81", null ],
          [ "2. Exponential-Golomb (Parameterized Family) ✓", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md82", null ],
          [ "3. Elias Omega ✓", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md83", null ]
        ] ],
        [ "Test Results", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md84", null ]
      ] ],
      [ "Succinct Structures - Phase 2 Foundation", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md85", [
        [ "1. Succinct Bit Vector with Rank/Select", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md86", [
          [ "Design Principles", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md87", null ],
          [ "Architecture", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md88", null ],
          [ "Block-Based Rank Support", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md89", null ],
          [ "Zero-Copy Wire Format", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md90", null ]
        ] ],
        [ "Example Usage", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md91", null ]
      ] ],
      [ "Phase 3: Advanced Succinct Structures", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md92", [
        [ "2. Roaring Bitmaps", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md93", [
          [ "Design Sketch", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md94", null ],
          [ "Zero-Copy Advantage", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md95", null ]
        ] ],
        [ "3. Interpolative Coding", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md96", [
          [ "Use Cases", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md97", null ]
        ] ],
        [ "4. Wavelet Tree", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md98", [
          [ "Applications", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md99", null ]
        ] ],
        [ "5. Packed Graphs", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md100", [
          [ "Zero-Copy Advantage", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md101", null ]
        ] ],
        [ "6. Rank-Select Dictionary", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md102", [
          [ "Use Case", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md103", null ]
        ] ]
      ] ],
      [ "Design Principles", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md104", [
        [ "1. Zero-Copy First", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md105", null ],
        [ "2. Composability", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md106", null ],
        [ "3. STL-Adjacent", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md107", null ],
        [ "4. Performance First", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md108", null ],
        [ "5. Immutable by Default", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md109", null ]
      ] ],
      [ "Integration with Existing PFC", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md110", [
        [ "Header Organization", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md111", null ],
        [ "Namespace Structure", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md112", null ],
        [ "PackedValue Integration", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md113", null ]
      ] ],
      [ "Performance Considerations", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md114", [
        [ "Space Overhead", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md115", null ],
        [ "Time Complexity", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md116", null ]
      ] ],
      [ "Testing Strategy", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md117", [
        [ "Unit Tests", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md118", null ],
        [ "Benchmark Tests", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md119", null ]
      ] ],
      [ "Roadmap", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md120", [
        [ "Phase 1: Quick Wins ✓ COMPLETED", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md121", null ],
        [ "Phase 2: Foundation (IN PROGRESS)", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md122", null ],
        [ "Phase 3: Core Structures (NEXT)", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md123", null ],
        [ "Phase 4: Advanced Features", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md124", null ],
        [ "Phase 5: Optimization", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md125", null ]
      ] ],
      [ "References", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md126", [
        [ "Papers", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md127", null ],
        [ "Libraries (for inspiration, not copying)", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md128", null ],
        [ "Standards", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md129", null ]
      ] ],
      [ "Conclusion", "md__2home_2runner_2work_2pfc_2pfc_2SUCCINCT__DESIGN.html#autotoc_md130", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Concepts", "concepts.html", "concepts" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"algebraic_8hpp.html",
"classpfc_1_1PackedContainer.html",
"classpfc_1_1PackedPoint.html",
"classpfc_1_1SafeBitReader.html#a6ef23bd439747a53ed6059e0f1246cb9",
"classpfc_1_1integrity_1_1CRC16.html",
"classpfc_1_1stream__bit__writer.html#af97dacf181aed250abd844934a214a2d",
"md__2home_2runner_2work_2pfc_2pfc_2NEW__CODECS__SUMMARY.html#autotoc_md62",
"structpfc_1_1OptimalCodec.html",
"structpfc_1_1compression_1_1LZ77Config.html#ad5c1d05e729864ad994bff5065643797"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';