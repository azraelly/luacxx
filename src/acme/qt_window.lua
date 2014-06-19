acme = acme or {};

function acme.qt_window(...)
    require "luacxx.QRect";
    require "luacxx.QGuiApplication";
    require "luacxx.QWindow";
    require "luacxx.QEventFilter";
    require "luacxx.QEvent";
    require "luacxx.QBackingStore";
    require "luacxx.QPainter";
    require "luacxx.QElapsedTimer";
    require "luacxx.QVector3D";
    require "luacxx.Qt";
    require "luacxx.QRegion";

    local qApp = QGuiApplication:new("demo", ...);

    window = QWindow:new();
    store = QBackingStore:new(window);

    local function render()
        if not window:isExposed() then
            return;
        end;
        if window:width() == 0 then
            return;
        end;

        local rect = QRect:new(0, 0, window:width(), window:height());

        store:beginPaint(QRegion:new(rect));

        local painter = QPainter:new();

        painter:begin(store:paintDevice());
        painter:fillRect(0, 0, window:width(), window:height(), Qt.white);
        painter:drawText(15, 15, "I love my Jenny!");
        painter:endPainting();

        store:endPaint();
        store:flush(QRegion:new(rect));
    end;

    window:event(function(event)
        if event:type() == QEvent.Expose then
            render();
        elseif event:type() == QEvent.Resize then
            store:resize(event:size());
            render();
        end;
        return false;
    end);
    window:show();

    return QGuiApplication.exec();
end;
