class Color
{
public:
    double r, g, b;

    Color() {}

    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    static Color setBlack()
    {
        Color color(0, 0, 0);
        return color;
    }

    //  Color needs to be between (0-1) for this offline
    void normalize()
    {
        if (r < 0)
            r = 0;
        if (g < 0)
            g = 0;
        if (b < 0)
            b = 0;
        if (r > 1)
            r = 1;
        if (g > 1)
            g = 1;
        if (b > 1)
            b = 1;
    }

    Color operator*(const double value)
    {
        return Color(this->r * value, this->g * value, this->b * value);
    }

    Color operator*(const Color color)
    {
        return Color(this->r * color.r, this->g * color.g, this->b * color.b);
    }

    Color operator+(const Color color)
    {
        return Color(this->r + color.r, this->g + color.g, this->b + color.b);
    }
};