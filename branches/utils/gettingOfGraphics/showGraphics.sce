PATH = '/home/atsarev/software/omnetpp-4.4.1/WORK/SelfSimTraffic/SelfSimTrafficForJitterMeasure/';

// Чтение вещественного числа из xml тега
function [field] = getDoubleFromXml(doc, xmlPath)
    xmlList = xmlXPath(doc, xmlPath);//take element from xmlPath
    field = strtod(xmlList(1).content);
endfunction

// Чтение большой строки чисел как вектор маленьких строк
function [vec] = getStrVector(doc, xmlPath, limit)
    xmlList = xmlXPath(doc, xmlPath);//take element from xmlPath
    bigString = xmlList(1).content;
    vec = strsplit(bigString(1), "  ", limit);
endfunction



//Функция вывода графиков одномерного распределения и АКФ из xml файлов
//Переменное число парметров (переменное количество входных файлов)
function drawingDistrAndACF(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawingDistrAndACF: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k как для одномерного распределения, так и для АКФ среди всех входных файлов
    k_MAX = -1;//максимальное значение k из всех файлов
    kMax = zeros(rhs, 1);
    k_MAX_ACF = -1;
    kMax_ACF = zeros(rhs, 1);
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Часть для одномерного распределения
        kMax(iter, 1) = getDoubleFromXml(doc, "//TRAFFIC-MAXVALUE/text()");//<TRAFFIC-MAXVALUE>...   сохраняем максимальное k для каждого файла
        k_MAX = max(k_MAX, kMax(iter, 1));
        
        //Часть для АКФ
        kMax_ACF(iter, 1) = getDoubleFromXml(doc, "//ACF-RANGE/text()"); 
        k_MAX_ACF = max(k_MAX_ACF, kMax_ACF(iter, 1))
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс (одного для всех с максимальной длиной) и заполнение нулями векторов ординат для каждого файла свой (и тоже они все максимальной длины, среди длин из разных файлов)
    //Для одномерного распределения
    k = zeros(k_MAX + 1, 1);//количество пакетов в системе (для одномерного распределения) от 0 до k_MAX
    for i = 1 : (k_MAX + 1)
        k(i,1) = i-1;
    end
    Pr = zeros(k_MAX + 1, rhs);//<TRAFFIC-DISTRIBUTION>... для rhs разных файлов
    
    //Для АКФ
    k_ACF = zeros(k_MAX_ACF, 1);//отсчёты времени для АКФ от 0 до k_MAX_ACF-1
    for i = 1 : (k_MAX_ACF)
        k_ACF(i,1) = i-1;
    end
    R = zeros(k_MAX_ACF, rhs);//<ACF-VALUES> ... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Чтение одномерного распределения
        strVec = getStrVector(doc, "//TRAFFIC-DISTRIBUTION/text()", kMax(iter) + 1);
        for i = 1 : (kMax(iter) + 1)
            Pr(i,iter) = strtod(strVec(i));// парсинг значений
        end
        
        //Чтение значений АКФ
        strVec_ACF = getStrVector(doc, "//ACF-VALUES/text()", kMax_ACF(iter));
        for i = 1 : kMax_ACF(iter)
            R(i,iter) = strtod(strVec_ACF(i));
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legDistrib = [varargin(1) + ':  TRAFFIC-DISTRIBUTION'];//запись легенды для графиков одномерного распределения
    legACF = [varargin(1) + ':  ACF'];//запись легенды для графиков АКФ
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legDistrib = [legDistrib ; (varargin(iter) + ':  TRAFFIC-DISTRIBUTION')];
        legACF = [legACF ; (varargin(iter) + ':  ACF')];
    end
    
    scf();//0
    plot2d(k, Pr, [grphColors]); 
    hl=legend(legDistrib);
    xtitle("Рапределение вероятностей траффиков");
    xgrid();
    
    scf();//1
    plot2d(k_ACF, R, [grphColors]);
    h2=legend(legACF);
    xtitle("АКФ траффиков");
    xgrid();
endfunction



//Функция вывода графиков одномерного распределения скорости источников из *_profile.xml файлов
//Переменное число парметров (переменное количество входных файлов)
function drawingSpeedDistr(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawingSpeedDistr: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k для одномерного распределения среди всех входных файлов
    k_MAX = -1;//максимальное значение k из всех файлов
    kMax = zeros(rhs, 1);
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //<MAXIMAL-SPEED>...   сохраняем максимальное k для каждого файла
        kMax(iter, 1) = getDoubleFromXml(doc, "//MAXIMAL-SPEED/text()");
        k_MAX = max(k_MAX, kMax(iter, 1));
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс (одного для всех с максимальной длиной) и заполнение нулями векторов ординат для каждого файла свой (и тоже они все максимальной длины, среди длин из разных файлов)
    k = zeros(k_MAX + 1, 1);//количество пакетов в системе (для одномерного распределения) от 0 до k_MAX
    for i = 1 : (k_MAX + 1)
        k(i,1) = i-1;
    end
    Pr = zeros(k_MAX + 1, rhs);//<MAXIMAL-SPEED>... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Чтение одномерного распределения
        strVec = getStrVector(doc, "//SPEED-DISTRIBUTION/text()", kMax(iter) + 1);
        for i = 1 : (kMax(iter) + 1)
            Pr(i,iter) = strtod(strVec(i));// парсинг значений
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legDistrib = [varargin(1) + ':  SPEED-DISTRIBUTION'];//запись легенды для графиков одномерного распределения
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legDistrib = [legDistrib ; (varargin(iter) + ':  SPEED-DISTRIBUTION')];
    end
    
    scf();//0
    plot2d(k, Pr, [grphColors]);  
    hl=legend(legDistrib);   
    xtitle("Распределение скорости источников");
    xgrid();
endfunction



function drawAllSpeedDistrib(folder)
    PATH = PATH + folder + '\';
    xmlFiles = getAppropriateFiles("*_profile.xml");
    xmlFiles = invert(xmlFiles);
    
    printf("Список фалов для скорости источников: "); disp(xmlFiles);
    printf("\n");    
    
    count = size(xmlFiles, 'r');
    for i = 1 : count
        drawingSpeedDistr(xmlFiles(i));
    end
endfunction



//Функция вывода гостограмм трафика из файлов *_traffic.xml файлов
//Переменное число парметров (переменное количество входных файлов)
function drawTrafficHistograms(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawTrafficHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k для ГИСТОГРАММ среди всех входных файлов
    k_MAX = -1;//максимальное значение k из всех файлов
    kMax = zeros(rhs, 1);
    widthOfCell = -1;
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //<NUM-HIST-CELLS-FOR-TRAFFIC>...   сохраняем максимальное k для каждого файла
        kMax(iter, 1) = getDoubleFromXml(doc, "//NUM-HIST-CELLS-FOR-TRAFFIC/text()");
        k_MAX = max(k_MAX, kMax(iter, 1));
        
        //если ширина ячейки в очередном файле отличается от предыдущего, то ошибка
        if (widthOfCell <> -1 & widthOfCell <> getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()")) then 
            error(msprintf("drawTrafficHistograms: различная ширина окна в файлах"));
        end
        widthOfCell = getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()");
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс (одного для всех с максимальной длиной) и заполнение нулями векторов ординат для каждого файла свой (и тоже они все максимальной длины, среди длин из разных файлов)
    //Для ГИСТОГРАММЫ вектор байт трафика.
    k = zeros(k_MAX + 1, 1);
    for i = 1 : (k_MAX + 1)
        k(i,1) = (i-1) * widthOfCell;
        //k(i,1) = ((i-1) + i) * widthOfCell / 2;//середины ячеек
    end
    Pr = zeros(k_MAX + 1, rhs);//<NUM-HIST-CELLS-FOR-TRAFFIC>... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Чтение ГИСТОГРАММЫ
        strVec = getStrVector(doc, "//HIST-POINTS/text()", kMax(iter) + 1);
        for i = 1 : (kMax(iter) + 1)
            Pr(i,iter) = strtod(strVec(i));// парсинг значений
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legDistrib = varargin(1) + ':  HIST-POINTS';//запись легенд
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legDistrib = legDistrib + '@' + varargin(iter) + ':  HIST-POINTS';
    end
    
    clf();
    //bar(k, Pr);//для этого графика нужны середины ячеек
    plot2d2(k, Pr, [grphColors], nax=[0, (k_MAX+1)/8+1, 0, 11], leg = legDistrib);
    plot2d3(k ,Pr, [grphColors], nax=[0, (k_MAX+1)/8+1, 0, 11], leg = legDistrib);
    xtitle("Гистограммы размера пакетов (ширина столбца = " + string(widthOfCell) + " байт)");
    xgrid();
endfunction



//Функция вывода гистограмм задержек из файлов *_jitter.xml файлов
//Переменное число параметров (переменное количество входных файлов)
//ДИАПАЗОНЫ ГИСТОГРАММ ДОЛЖНЫ БЫТЬ ОДИНАКОВЫ!!!
function drawAllJitterHistogramms(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawAllJitterHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k для ГИСТОГРАММ среди всех входных файлов
    k_MAX = -1;//максимальное значение k из всех файлов
    kMax = zeros(rhs, 1);
    widthOfCell = -1;
    leftBound = -1;
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        kMax(iter, 1) = getDoubleFromXml(doc, "//NUM-HIST-CELLS/text()");//take element from <NUM-HIST-CELLS> ...</>
        k_MAX = max(k_MAX, kMax(iter, 1));
        
        //если ширина ячейки в очередном файле отличается от предыдущего, то ошибка
        if (widthOfCell <> -1 & widthOfCell <> getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()")) then 
            error(msprintf("drawAllJitterHistograms: различная ширина окна в файлах"));
        end
        widthOfCell = getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()");
        
        strVec = getStrVector(doc, "//RANGE/text()", 2);//take element from <RANGE> ...</>
        if (leftBound <> -1 & leftBound <> strtod(strVec(1))) then
            error(msprintf("drawAllJitterHistograms: различная левая граница в файлах"));
        end
        leftBound = strtod(strVec(1));
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс (одного для всех с максимальной длиной) и заполнение нулями векторов ординат для каждого файла свой (и тоже они все максимальной длины, среди длин из разных файлов)
    t = zeros(k_MAX);
    t(1) = (leftBound + (leftBound + widthOfCell))/2;
    for i = 2 : k_MAX
        t(i) = t(i-1) + widthOfCell;//середины ячеек
    end
    hist = zeros(k_MAX, rhs);//<PDFVALUES>... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Чтение ГИСТОГРАММЫ
        strVec = getStrVector(doc, "//PDF-VALUES/text()", kMax(iter));//take vakues <PDFVALUES>...</> 
        for i = 1 : kMax(iter)
            hist(i,iter) = strtod(strVec(i));// парсинг значений
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legDistrib = [varargin(1) + ':  PDF-VALUES'];//запись легенд
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legDistrib = [legDistrib ; (varargin(iter) + ':  PDF-VALUES') ];
    end
    
    clf();
    bar(t, hist);//для этого графика нужны середины ячеек
    hl=legend(legDistrib);
    xtitle("Гистограмма задержки пакетов (ширина столбца = " + string(widthOfCell) + " сек.");
    xgrid();
endfunction


function drawAllJitterPolygons(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawAllJitterHistograms: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k для ГИСТОГРАММ среди всех входных файлов
    k_MAX = -1;//максимальное значение k из всех файлов
    kMax = zeros(rhs, 1);
    widthOfCell = -1;
    leftBound = -1;
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        kMax(iter, 1) = getDoubleFromXml(doc, "//NUM-HIST-CELLS/text()");//take element from <NUM-HIST-CELLS> ...</>
        k_MAX = max(k_MAX, kMax(iter, 1));
        
        //если ширина ячейки в очередном файле отличается от предыдущего, то ошибка
        if (widthOfCell <> -1 & widthOfCell <> getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()")) then 
            error(msprintf("drawAllJitterHistograms: различная ширина окна в файлах"));
        end
        widthOfCell = getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()");
        
        strVec = getStrVector(doc, "//RANGE/text()", 2);//take element from <RANGE> ...</>
        if (leftBound <> -1 & leftBound <> strtod(strVec(1))) then
            error(msprintf("drawAllJitterHistograms: различная левая граница в файлах"));
        end
        leftBound = strtod(strVec(1));
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс (одного для всех с максимальной длиной) и заполнение нулями векторов ординат для каждого файла свой (и тоже они все максимальной длины, среди длин из разных файлов)
    t = zeros(k_MAX);
    t(1) = (leftBound + (leftBound + widthOfCell))/2;
    for i = 2 : k_MAX
        t(i) = t(i-1) + widthOfCell;//середины ячеек
    end
    hist = zeros(k_MAX, rhs);//<PDFVALUES>... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //Чтение ГИСТОГРАММЫ
        strVec = getStrVector(doc, "//PDF-VALUES/text()", kMax(iter));//take vakues <PDFVALUES>...</> 
        for i = 1 : kMax(iter)
            hist(i,iter) = strtod(strVec(i));// парсинг значений
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legDistrib = [varargin(1) + ':  PDF-VALUES'];//запись легенд
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legDistrib = [legDistrib ; (varargin(iter) + ':  PDF-VALUES') ];
    end 
    
    clf ();
    plot2d(t, hist, [grphColors]);    
    hl=legend(legDistrib);
    xtitle("Полигоны гистограмм для задержки пакетов (ширина столбца = " + string(widthOfCell) + " сек.");
    xgrid();
endfunction



//Функция вывода полигона гистограммы трафика из ОДНОГО файла *_jitter.xml файлов
function drawJitterPolygon(filename)
    doc = xmlRead(PATH + filename);
    numHistCells = getDoubleFromXml(doc, "//NUM-HIST-CELLS/text()");//take element from <NUM-HIST-CELLS> ...</>
    widthOfCell = getDoubleFromXml(doc, "//WIDTH-OF-CELL/text()");//take element from <WIDTH-OF-CELL> ...</>
    
    x = zeros(numHistCells);//абсциссы - средние значения интервалов гистограммы
    strVec = getStrVector(doc, "//CELL-CENTER-POINTS/text()", numHistCells);//take vakues <CELL-CENTER-POINTS>...</> 
    for i = 1 : numHistCells
        x(i) = strtod(strVec(i));// парсинг значений
    end
    
    y = zeros(numHistCells);//ординаты - значения гистограммы
    strVec = getStrVector(doc, "//PDF-VALUES/text()", numHistCells);//take vakues <PDF-VALUES>...</>
    for i = 1 : numHistCells
        y(i) = strtod(strVec(i));// парсинг значений
    end
    
    plot2d(x, y, 5);
    hl=legend(filename + ": PDF VALUES");
    xtitle("Полигон гистограммы задержки пакетов (ширина столбца = " + string(widthOfCell) + " сек.");
    xgrid();
endfunction



function drawAllJitterACF(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawAllJitterACF: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения k для АКФ среди всех входных файлов
    k_MAX_ACF = -1;
    kMax_ACF = zeros(rhs, 1);
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        kMax_ACF(iter, 1) = getDoubleFromXml(doc, "//ACF-RANGE/text()"); 
        k_MAX_ACF = max(k_MAX_ACF, kMax_ACF(iter, 1))
        
        xmlDelete(doc);
    end
    
    //Заполнение вектора абсцисс 
    k_ACF = zeros(k_MAX_ACF, 1);//отсчёты времени для АКФ от 0 до k_MAX_ACF-1
    for i = 1 : (k_MAX_ACF)
        k_ACF(i,1) = i-1;
    end
    R = zeros(k_MAX_ACF, rhs);//<ACF-VALUES> ... для rhs разных файлов
    
    //Чтение значений ординат для векторов
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));

        strVec_ACF = getStrVector(doc, "//ACF-VALUES/text()", kMax_ACF(iter));
        for i = 1 : kMax_ACF(iter)
            R(i,iter) = strtod(strVec_ACF(i));
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];
    legACF = [varargin(1) + ':  ACF'];
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legACF = [legACF ; (varargin(iter) + ':  ACF')];
    end
    
    scf();
    plot2d(k_ACF, R, [grphColors]);
    hl=legend(legACF);
    xtitle("График АКФ джиттера");
    xgrid();
endfunction



function drawQueueSizeGraphic(varargin)
    //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawQueueSizeGraphic: Ожидалось один или более параметров (имён файлов)"));
    end
    
    //Нахождение максимального значения rightBound среди всех входных файлов
    sizeOfVector_MAX = -1;//максимальное значение rightBound из всех файлов
    leftBound = -1;
    sizeOfVector = zeros(rhs, 1);
    minTimeSlot = -1;
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        //если левая граница диапазона в очередном файле отличается от предыдущего, то ошибка
        range_ = getStrVector(doc, "//RANGE/text()", 2);
        if (leftBound <> -1 & leftBound <> strtod(range_(1))) then
            error(msprintf("drawQueueSizeGraphic: различная левая граница в файлах"));
        end
        leftBound = strtod(range_(1));
        
        //если минимальный временной слот в очередном файле отличается от предыдущего, то ошибка
        if (minTimeSlot <> -1 & minTimeSlot <> getDoubleFromXml(doc, "//GRAPHIC-TIME-SLOT/text()")) then 
            error(msprintf("drawAllJitterHistograms: различный минимальный временной слот в файлах"));
        end
        minTimeSlot = getDoubleFromXml(doc, "//GRAPHIC-TIME-SLOT/text()");
        
        sizeOfVector(iter, 1) = getDoubleFromXml(doc, "//SIZE-OF-VECTOR/text()");
        sizeOfVector_MAX = max(sizeOfVector_MAX, sizeOfVector(iter, 1)); 

        xmlDelete(doc);
    end
    
    //Чтение значений абсциссы для векторов (из первого файла, предполагая что абсциссы одинаковы во всех файлах (ПРОВЕРЯТЬ ВРУЧНУЮ)). 
    //ЕСЛИ ЭТО НЕ ТАК, ТО ПОЛЬЗОВАТЬСЯ МЕТОДОМ ДЛЯ НЕСКОЛЬКИХ ФАЙЛОВ ОДНОВРЕМЕННО НЕЛЬЗЯ!!!
    timePoints = zeros(sizeOfVector_MAX, 1);
    doc = xmlRead(PATH + varargin(1));
        
    strVec = getStrVector(doc, "//TIME-POINTS/text()", sizeOfVector_MAX);
    for i = 1 : sizeOfVector_MAX
        timePoints(i, 1) = strtod(strVec(i));// парсинг значений
    end
    xmlDelete(doc);

    //Чтение значений ординат для векторов
    sizePoints = zeros(sizeOfVector_MAX, rhs);
    for iter = 1 : rhs
        doc = xmlRead(PATH + varargin(iter));
        
        strVec = getStrVector(doc, "//SIZE-POINTS/text()", sizeOfVector(iter));
        for i = 1 : sizeOfVector(iter)
            sizePoints(i, iter) = strtod(strVec(i));// парсинг значений
        end
        
        xmlDelete(doc);
    end
    
    //Запись различных цветов для графиков и легенды
    grphColors = [3];//запись цветов для графиков
    legenda = [varargin(1) + ': SIZE-OF-VECTOR = ' + string(sizeOfVector(1, 1))];
    for iter = 2 : rhs
        grphColors = [grphColors (grphColors(iter-1)+2)];
        legenda = [legenda ; (varargin(iter) + ': SIZE-OF-VECTOR = ' + string(sizeOfVector(iter, 1)))];
    end
    
    scf();//0
    plot2d(timePoints, sizePoints, [grphColors]);
    hl=legend(legenda);
    xtitle("График размера очереди/буфера. Минимальный размер отсчётов: " + string(minTimeSlot) + " сек.");
    xgrid();
endfunction




//----------------------------------Jitter Statistics---------------------------

//рисует графики зависимости вероятности НЕСГЛАЖЕННОГО джиттера от FIRST-DELAY
function drawJitterProbByDelay(varargin)
     //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawJitterProbByDelay: Ожидалось один или более параметров (имён папок)"));
    end
    
    scf();//0
    legenda = [];
    for iter = 1 : rhs
        [firstDelaysRelative, numOfQueues, bufferSize, withCounterReset, Probabilities] = getInfoForGraphic(varargin(iter));
        plot2d(firstDelaysRelative, Probabilities, (1 + 2 * iter));
        
        legenda = [legenda ; (varargin(iter) + ": Число узлов = " + string(numOfQueues(1))) ];
    end
    
    hl=legend(legenda);
    xtitle("График вероятности не сглаженного джиттера в зависимости от первой задержки");
    xgrid();
endfunction

//рисует графики зависимости вероятности НЕСГЛАЖЕННОГО джиттера от FIRST-DELAY
function drawJitterProbByBufSize(varargin)
     //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawJitterProbByDelay: Ожидалось один или более параметров (имён папок)"));
    end
    
    scf();//0
    legenda = [];
    for iter = 1 : rhs
        [firstDelaysRelative, numOfQueues, bufferSize, withCounterReset, Probabilities] = getInfoForGraphic(varargin(iter));
        disp(bufferSize);
        disp(Probabilities);
        plot2d(bufferSize, Probabilities, (1 + 2 * iter));
        
        legenda = [legenda ; (varargin(iter) + ": N = " + string(numOfQueues(1)) + ", FD = " + string(firstDelaysRelative(1)) + ", WithCReset = " + string(withCounterReset(1))) ];
    end
    
    hl=legend(legenda);
    xtitle("График вероятности не сглаженного джиттера в зависимости от РАЗМЕРА БУФЕРА");
    xgrid();
endfunction

//рисует графики зависимости вероятности НЕ СГЛАЖЕННОГО джиттера от NUMBER-OF-COMPOUND-QUEUES
function drawJitterProbByQueueNum(varargin)
     //СЧИТЫВАНИЕ ПАРАМЕТРОВ
    [lhs, rhs] = argn();// rhs - количество входных параметров
    if (rhs < 1) then
        error(msprintf("drawJitterProbByDelay: Ожидалось один или более параметров (имён папок)"));
    end
    
    scf();//0
    legenda = [];
    for iter = 1 : rhs
        [firstDelaysRelative, numOfQueues, bufferSize, withCounterReset, Probabilities] = getInfoForGraphic(varargin(iter));
        plot2d(numOfQueues, Probabilities, (1 + 2 * iter));
        legenda = [legenda ; (varargin(iter) + ": Величина первой задержки = " + string(firstDelaysRelative(1)) + " ms.")];
    end
    
    hl=legend(legenda);
    xtitle("График вероятности не сглаженного джиттера в зависимоти от количества узлов");
    xgrid();
endfunction

//Функция считывающая необходимые скалярные величины из ВСЕХ файлов типа *_JitterAfterBuffer.xml в УКАЗАННОЙ ПАПКЕ
function [firstDelaysRelative, numOfQueues, bufferSize, withCounterReset, Probabilities] = getInfoForGraphic(folder)
    TMP_PATH = PATH + folder + '\';
    cd(TMP_PATH);
    xmlFiles = ls("*_JitterAfterBuffer.xml");
    xmlFiles = invert(xmlFiles);
    printf("Список фалов: "); 
    disp(xmlFiles);
    printf("\n");
    
    fileCount = size(xmlFiles, 1);
    if (fileCount < 1) then
        error(msprintf("processAllFiles: нет файлов для обработки"));
    end
    
    firstDelaysRelative = zeros(fileCount, 1);
    numOfQueues = zeros(fileCount, 1);
    bufferSize = zeros(fileCount, 1);
    withCounterReset = zeros(fileCount, 1);
    Probabilities = zeros(fileCount, 1);
    
    for i = 1 : fileCount
        doc = xmlRead(TMP_PATH + xmlFiles(i));

        firstDelaysRelative(i) = getDoubleFromXml(doc, "//FIRSTDELAY-DIV-INTERTIME/text()");
        numOfQueues(i) = getDoubleFromXml(doc, "//NUMBER-OF-COMPOUND-QUEUES/text()"); 
        bufferSize(i) = getDoubleFromXml(doc, "//BUFFER-SIZE/text()"); 
        withCounterReset(i) = getDoubleFromXml(doc, "//WITH-COUNTER-RESET/text()"); 
        Probabilities(i) = getDoubleFromXml(doc, "//PROBABILITY-OF-UNSMOOTHED-JITTER/text()");
    end
endfunction

//Инвертируем массив-столбец
function [invX] = invert(x)
    n = size(x, 'r');
    invX = [];
    for (i = 1 : n )
        invX = [invX ; x(n - i + 1)];
    end
endfunction


//---------------------------------------------------------------------------------------------------------------------
//Функция для расчёта S(x,y) для последующего построения графика

FIT_POINTS = 4; //early was 4 !!!
IMAX = 100;

function [h] = differ(x, y, H, nacf)
    
    i = 0; k = 0; n = 0; imax = 0;
    //индексы массивов начинаются с 1, поэтому некоторые изменения в коде, относительно С++ого
    s = 0; s2 = 0; ss = zeros(1, FIT_POINTS); nr = zeros(1, FIT_POINTS); alpha=4-2*H;
    
    h = (2+x)^(-alpha);
    s = h;
    n = 3;
    while (h/s > 1.e-6)
        h = (n+x)^(-alpha);
        s = s + h;
        n = n + 1; 
    end
    imax = n;
    s2 = s;
    
    ss(1, 1) = s;
    i = 2;
    while (i <= IMAX & i < imax)
        s = s - (i+x)^(-alpha);
        for k = 1 : FIT_POINTS
            if i>=k then
                ss(1, k) = ss(1, k) + s;
            end
        end
        i = i + 1;
    end
    
    h = 0;
    for k = 1 : FIT_POINTS
        nr(1, k) = (1-y)*ss(1, k)/( s2+(1-y)*ss(1, 1) );
        h = h + (nacf(1, k+1) - nr(1, k))^2;
        k = k + 1;
    end
endfunction

function main()
    //Считывание параметра H и значений АКФ
    doc_new = xmlRead(PATH + "FromFile_traffic.xml");

    xmlList_new = xmlXPath(doc_new, "//HURST/text()");//take element from <HURST> ...</>
    H = strtod(xmlList_new(1).content);//Hurst parametr

    acfRange_str = xmlXPath(doc_new, "//ACF-RANGE/text()");
    acfRange = strtod(acfRange_str(1).content);
    xmlList_new = xmlXPath(doc_new, "//ACF-VALUES/text()");
    acfValues_str_new = xmlList_new(1).content;
    strVec_ACF_new = strsplit(acfValues_str_new(1), "  ", acfRange);//разделение одной большой строки на массив строк со значениями

    nacf = zeros(1, acfRange);//Vector of ACF's values
    for k = 1 : acfRange
        nacf(1, k) = strtod(strVec_ACF_new(k));
    end
    
    //Построение графика
    x = [-1.9 : 0.1 : 10]'; //должно выполнятся x>-2
    y = [0 : 0.001 : 1]'; //   0 <= y <= 1
    sizeX = size(x, 1);
    sizeY = size(y, 1);
    S = zeros(sizeX, sizeY);
    for i = 1 : sizeX
        for j = 1 : sizeY
            S(i,j) = differ(x(i), y(j), H, nacf);
        end
    end
    
    plot3d(x, y, S, leg =  "X@Y@Z");

endfunction
