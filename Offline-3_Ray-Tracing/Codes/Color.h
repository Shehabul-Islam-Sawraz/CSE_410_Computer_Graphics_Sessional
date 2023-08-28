class Color
{
public:
    int r, g, b;

    Color() {}

    Color(int r, int g, int b)
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
};