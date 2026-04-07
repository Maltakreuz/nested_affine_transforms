void print_gpu_info() {
    cout << "=== GPU DETAILED INFO ===" << endl;
    cout << "Vendor   : " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer : " << glGetString(GL_RENDERER) << endl;
    cout << "Version  : " << glGetString(GL_VERSION) << endl;
    cout << "GLSL     : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    GLint v;

    // --- ТЕКСТУРЫ ---
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &v);
    cout << "Max Texture Units (Fragment): " << v << " (сколько 'розеток' в шейдере)" << endl;

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &v);
    cout << "Max Combined Texture Units  : " << v << " (всего юнитов для всех стадий)" << endl;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &v);
    cout << "Max Texture Size            : " << v << " px" << endl;

    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &v);
    cout << "Max Texture Array Layers    : " << v << " (слоев в одном Texture Array)" << endl;

    // --- ПАМЯТЬ И БУФЕРЫ (Тут мощь 4090) ---
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &v);
    cout << "Max Uniform Block Size      : " << v / 1024 << " KB (размер UBO)" << endl;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &v);
    cout << "Max Vertex Attributes       : " << v << " (данных на одну вершину)" << endl;

    // --- COMPUTE (Вычисления) ---
    // Это лимиты для ядер, если использовать GPU для математики (Compute Shaders)
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &v);
    cout << "Max Compute Work Group Invoc: " << v << " (потоков в одной группе)" << endl;

    // --- FRAMEBUFFER (Отрисовка в текстуру) ---
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v);
    cout << "Max Color Attachments       : " << v << " (сколько текстур рисовать за раз)" << endl;

    glGetIntegerv(GL_MAX_SAMPLES, &v);
    cout << "Max MSAA Samples            : " << v << "x (макс. сглаживание)" << endl;

    cout << "=========================" << endl;
}


void print_gpu_mega_info() {
    GLint v[3]; // Массив, так как некоторые параметры возвращают 3 числа (X, Y, Z)
    
    cout << "\n=== [ ПАМЯТЬ И ГЕОМЕТРИЯ ] ===" << endl;
    
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &v[0]);
    cout << "Max Elements Vertices      : " << v[0] << " (рекомендуемый предел вершин за 1 вызов)" << endl;

    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &v[0]);
    cout << "Max Elements Indices       : " << v[0] << " (рекомендуемый предел индексов)" << endl;

    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &v[0]);
    cout << "Max Vertex Output Comp.    : " << v[0] << " (сколько данных Vertex Shader может передать в Fragment)" << endl;

    cout << "\n=== [ ВЫЧИСЛИТЕЛЬНАЯ МОЩЬ (COMPUTE) ] ===" << endl;
    
    // Как много ядер могут работать вместе над одной задачей
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, v); // Возвращает 3 значения
    cout << "Max Compute Work Groups    : X:" << v[0] << " Y:" << v[1] << " Z:" << v[2] << endl;
    
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, v);
    cout << "Max Work Group Size        : X:" << v[0] << " Y:" << v[1] << " Z:" << v[2] << endl;

    cout << "\n=== [ ТЕКСТУРЫ И БУФЕРЫ (ADVANCED) ] ===" << endl;

    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &v[0]);
    cout << "Max Texture Buffer Size    : " << v[0] / (1024 * 1024) << " MB (огромный массив данных как текстура)" << endl;

    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &v[0]);
    cout << "Max Uniform Locations      : " << v[0] << " (макс. количество имен переменных в шейдере)" << endl;

    // В OpenGL можно "мапить" память GPU в адресное пространство CPU
    GLint64 v64;
    glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &v64);
    cout << "Max Server Wait Timeout    : " << v64 << " ns" << endl;

    cout << "\n=== [ VIEWPORT И ЭКРАН ] ===" << endl;
    
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, v);
    cout << "Max Viewport Dimensions    : " << v[0] << " x " << v[1] << " px" << endl;

    float range[2];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
    cout << "Line Width Range           : " << range[0] << " to " << range[1] << " px" << endl;

    cout << "\n================================" << endl;
}

void print_gpu_ultimate_info() {
    GLint v;

    cout << "\n=== [ ШЕЙДЕРНЫЕ КОНСТАНТЫ (UNIFORMS) ] ===" << endl;
    
    // Лимиты для обычных uniform (те, что через glUniform... передаются)
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &v);
    cout << "Max Vertex Uniform Comp.    : " << v << " (float-компонент в Vertex Shader)" << endl;
    
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &v);
    cout << "Max Fragment Uniform Comp.  : " << v << " (float-компонент в Fragment Shader)" << endl;

    cout << "\n=== [ БУФЕРЫ ХРАНЕНИЯ (SSBO) - ЭТО МОЩЬ ] ===" << endl;
    // SSBO (Shader Storage Buffer Objects) — это как Uniform, но почти без лимитов по размеру
    
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &v);
    cout << "Max SSBO Bindings           : " << v << " (сколько огромных буферов можно подключить)" << endl;
    
    GLint64 v64;
    glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &v64);
    cout << "Max SSBO Block Size         : " << v64 / (1024 * 1024) << " MB (макс. размер одного такого буфера)" << endl;

    cout << "\n=== [ ПАРАМЕТРЫ РАСТЕРИЗАЦИИ ] ===" << endl;
    
    glGetIntegerv(GL_MAX_CLIP_DISTANCES, &v);
    cout << "Max Clip Distances          : " << v << " (плоскости отсечения геометрии)" << endl;

    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &v);
    cout << "Max Draw Buffers            : " << v << " (одновременных целей рендеринга)" << endl;

    // Слойность
    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &v);
    cout << "Max Framebuffer Layers      : " << v << endl;

    cout << "\n=== [ ПРЕДЕЛЫ СТЕКА И ОПЕРАЦИЙ ] ===" << endl;
    
    glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &v);
    cout << "Max Debug Logged Messages   : " << v << " (сколько ошибок OpenGL хранит в очереди)" << endl;

    cout << "========================================" << endl;
}

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

void print_nv_memory_info() {
    GLint total_mem_kb = 0;
    GLint cur_avail_mem_kb = 0;
    
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
    
    cout << "VRAM Total     : " << total_mem_kb / 1024 << " MB" << endl;
    cout << "VRAM Available : " << cur_avail_mem_kb / 1024 << " MB" << endl;
}

void print_all_gpu_info() {
    print_gpu_info();
    print_gpu_mega_info();
    print_gpu_ultimate_info();
    print_nv_memory_info();
}