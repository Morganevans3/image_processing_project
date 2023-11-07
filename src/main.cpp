//
// Created by Morgan Evans on 3/9/23.
//
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    Pixel(unsigned char blue, unsigned char green, unsigned char red) {
        this->blue = blue;
        this->green = green;
        this->red = red;
    }
};
struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
    vector<Pixel> pixels;
};



Header read_file(const char *file_name) {
    ifstream file(file_name, ios_base::in | ios_base::binary);
    if (!file.is_open()) {
        cout << "File was never opened!" << endl;
    }

    Header header;
    file.read(&header.idLength, 1);
    file.read(&header.colorMapType, 1);
    file.read(&header.dataTypeCode, 1);
    file.read((char*)(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
    file.read((char*)(&header.colorMapLength), sizeof(header.colorMapLength));
    file.read(&header.colorMapDepth, 1);
    file.read((char*)(&header.xOrigin), sizeof(header.xOrigin));
    file.read((char*)(&header.yOrigin), sizeof(header.yOrigin));
    file.read((char*)(&header.width), sizeof(header.width));
    file.read((char*)(&header.height), sizeof(header.height));
    file.read(&header.bitsPerPixel, 1);
    file.read(&header.imageDescriptor, 1);

    unsigned int size_image = header.height * header.width;

    for (int i = 0; i < size_image; i++) {

        char blue;
        char green;
        char red;
        file.read((char*)(&blue), sizeof(blue));
        file.read((char*)(&green), sizeof(green));
        file.read((char*)(&red), sizeof(red));
        Pixel pixel(blue, green, red);
        header.pixels.push_back(pixel);
    }

    return header;
}

void write_file(Header header, const char *written_file) {
    ofstream new_file(written_file, ios_base::out | ios_base::binary);

    new_file.write(&header.idLength, 1);
    new_file.write(&header.colorMapType, 1);
    new_file.write(&header.dataTypeCode, 1);
    new_file.write((char*)(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
    new_file.write((char*)(&header.colorMapLength), sizeof(header.colorMapLength));
    new_file.write(&header.colorMapDepth, 1);
    new_file.write((char*)(&header.xOrigin), sizeof(header.xOrigin));
    new_file.write((char*)(&header.yOrigin), sizeof(header.yOrigin));
    new_file.write((char*)(&header.width), sizeof(header.width));
    new_file.write((char*)(&header.height), sizeof(header.height));
    new_file.write(&header.bitsPerPixel, 1);
    new_file.write(&header.imageDescriptor, 1);

    for (int i = 0; i < header.width * header.height; i++) {
        new_file.write(reinterpret_cast<const char *>(&header.pixels[i].blue), sizeof(header.pixels[i].blue));
        new_file.write(reinterpret_cast<const char *>(&header.pixels[i].green), sizeof(header.pixels[i].green));
        new_file.write(reinterpret_cast<const char *>(&header.pixels[i].red), sizeof(header.pixels[i].red));
    }

    new_file.close();
}

float clamp(float num) {
    if (num < 0) {
        num = 0;
    }
    else if (num > 255) {
        num = 255;
    }
    return num;
}

void multiply(Header p1, Header p2, const char *part_file) {
    Header mult_header = p1;
    mult_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = ((int)p1.pixels[i].blue / 255.0f) * ((int)p2.pixels[i].blue / 255.0f) * 255 + 0.5f;
        valG = ((int)p1.pixels[i].green / 255.0f) * ((int)p2.pixels[i].green / 255.0f) * 255 + 0.5f;
        valR = ((int)p1.pixels[i].red / 255.0f) * ((int)p2.pixels[i].red / 255.0f) * 255 + 0.5f;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        mult_header.pixels.push_back(pixel);
    }

    write_file(mult_header, part_file);
}

Header multiply_load(const char *P1, const char *P2) {
    Header p1 = read_file(P1);
    Header p2 = read_file(P2);
    Header mult_header = p1;
    mult_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = ((int)p1.pixels[i].blue / 255.0f) * ((int)p2.pixels[i].blue / 255.0f) * 255 + 0.5f;
        valG = ((int)p1.pixels[i].green / 255.0f) * ((int)p2.pixels[i].green / 255.0f) * 255 + 0.5f;
        valR = ((int)p1.pixels[i].red / 255.0f) * ((int)p2.pixels[i].red / 255.0f) * 255 + 0.5f;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        mult_header.pixels.push_back(pixel);
    }

    return mult_header;
}

void screen(Header p1, Header p2, const char *part_file) {
    Header scr_header = p1;
    scr_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (1 - ((1 - ((int)p1.pixels[i].blue / 255.0f)) * (1 - ((int)p2.pixels[i].blue / 255.0f)))) * 255 + 0.5f;
        valG = (1 - ((1 - ((int)p1.pixels[i].green / 255.0f)) * (1 - ((int)p2.pixels[i].green / 255.0f)))) * 255 + 0.5f;
        valR = (1 - ((1 - ((int)p1.pixels[i].red / 255.0f)) * (1 - ((int)p2.pixels[i].red / 255.0f)))) * 255 + 0.5f;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        scr_header.pixels.push_back(pixel);
    }

    write_file(scr_header, part_file);
}


void subtract(Header p1, Header p2, const char *part_file) {
    Header sub_header = p1;
    sub_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue - (int)p2.pixels[i].blue;
        valG = (int)p1.pixels[i].green - (int)p2.pixels[i].green;
        valR = (int)p1.pixels[i].red - (int)p2.pixels[i].red;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        sub_header.pixels.push_back(pixel);
    }

    write_file(sub_header, part_file);
}

void addblue(Header p1, float num, const char *part_file) {
    Header b_add_header = p1;
    b_add_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue + num;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        b_add_header.pixels.push_back(pixel);
    }

    write_file(b_add_header, part_file);
}

void addgreen(Header p1, float num, const char *part_file) {
    Header g_add_header = p1;
    g_add_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        valG = (int)p1.pixels[i].green + num;
        valR = (int)p1.pixels[i].red;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        g_add_header.pixels.push_back(pixel);
    }

    write_file(g_add_header, part_file);
}

void addred(Header p1, float num, const char *part_file) {
    Header r_add_header = p1;
    r_add_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red + num;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        r_add_header.pixels.push_back(pixel);
    }

    write_file(r_add_header, part_file);
}


void overlay(Header p1, Header p2, const char *part_file) {
    Header overlay_header = p1;
    overlay_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        if ((int)p2.pixels[i].blue / 255.0f <= 0.5) {
            valB = 2 * ((int) p1.pixels[i].blue / 255.0f) * ((int) p2.pixels[i].blue / 255.0f) * 255 + 0.5f;
        }
        else if ((int)p2.pixels[i].blue / 255.0f > 0.5) {
            valB = (1 - (2 * (1 - ((int)p1.pixels[i].blue / 255.0f)) * (1 - ((int)p2.pixels[i].blue / 255.0f)))) * 255 + 0.5f;
        }
        if ((int)p2.pixels[i].green / 255.0f <= 0.5) {
            valG = 2 * ((int) p1.pixels[i].green / 255.0f) * ((int) p2.pixels[i].green / 255.0f) * 255 + 0.5f;
        }
        else if ((int)p2.pixels[i].green / 255.0f > 0.5) {
            valG = (1 - (2 * (1 - ((int)p1.pixels[i].green / 255.0f)) * (1 - ((int)p2.pixels[i].green / 255.0f)))) * 255 + 0.5f;
        }
        if ((int)p2.pixels[i].red / 255.0f <= 0.5) {
            valR = 2 * ((int) p1.pixels[i].red / 255.0f) * ((int) p2.pixels[i].red / 255.0f) * 255 + 0.5f;
        }
        else if ((int)p2.pixels[i].red / 255.0f > 0.5) {
            valR = (1 - (2 * (1 - ((int)p1.pixels[i].red / 255.0f)) * (1 - ((int)p2.pixels[i].red / 255.0f)))) * 255 + 0.5f;
        }

        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        overlay_header.pixels.push_back(pixel);
    }

    write_file(overlay_header, part_file);
}

void scale(Header p1, float num1, float num2, const char *part_file) {
    Header scale_header = p1;
    scale_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue * num2;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red * num1;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        scale_header.pixels.push_back(pixel);
    }

    write_file(scale_header, part_file);
}

void scaleblue(Header p1, float num, const char *part_file) {
    Header b_scale_header = p1;
    b_scale_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue * num;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        b_scale_header.pixels.push_back(pixel);
    }

    write_file(b_scale_header, part_file);
}

void scalegreen(Header p1, float num, const char *part_file) {
    Header g_scale_header = p1;
    g_scale_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        valG = (int)p1.pixels[i].green * num;
        valR = (int)p1.pixels[i].red;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        g_scale_header.pixels.push_back(pixel);
    }

    write_file(g_scale_header, part_file);
}

void scalered(Header p1, float num, const char *part_file) {
    Header r_scale_header = p1;
    r_scale_header.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red * num;
        valB = clamp(valB);
        valG = clamp(valG);
        valR = clamp(valR);
        Pixel pixel((int)valB, (int)valG, (int)valR);
        r_scale_header.pixels.push_back(pixel);
    }

    write_file(r_scale_header, part_file);
}

void separate(Header p1, const char *part_file1, const char *part_file2, const char *part_file3) {
    Header s1 = p1;
    Header s2 = p1;
    Header s3 = p1;
    s1.pixels.clear();
    s2.pixels.clear();
    s3.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        valG = (int)p1.pixels[i].green;
        valR = (int)p1.pixels[i].red;
        Pixel pixel1((int)valR, (int)valR, (int)valR);
        s1.pixels.push_back(pixel1);
        Pixel pixel2((int)valG, (int)valG, (int)valG);
        s2.pixels.push_back(pixel2);
        Pixel pixel3((int)valB, (int)valB, (int)valB);
        s3.pixels.push_back(pixel3);
    }

    write_file(s1, part_file1);
    write_file(s2, part_file2);
    write_file(s3, part_file3);
}

void onlyblue(Header p1, const char *part_file) {
    Header s1 = p1;
    s1.pixels.clear();
    float valB;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p1.pixels[i].blue;
        Pixel pixel1((int)valB, (int)valB, (int)valB);
        s1.pixels.push_back(pixel1);
    }

    write_file(s1, part_file);
}

void onlygreen(Header p1, const char *part_file) {
    Header s1 = p1;
    s1.pixels.clear();
    float valG;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valG = (int)p1.pixels[i].green;
        Pixel pixel1((int)valG, (int)valG, (int)valG);
        s1.pixels.push_back(pixel1);
    }

    write_file(s1, part_file);
}

void onlyred(Header p1, const char *part_file) {
    Header s1 = p1;
    s1.pixels.clear();
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valR = (int)p1.pixels[i].red;
        Pixel pixel1((int)valR, (int)valR, (int)valR);
        s1.pixels.push_back(pixel1);
    }

    write_file(s1, part_file);
}

void combine(Header p1, Header p2, Header p3, const char *part_file) {
    Header com = p1;
    com.pixels.clear();
    float valB;
    float valG;
    float valR;
    for (int i = 0; i < p1.width * p1.height; i++) {
        valB = (int)p3.pixels[i].blue;
        valG = (int)p2.pixels[i].green;
        valR = (int)p1.pixels[i].red;
        Pixel pixel((int)valB, (int)valG, (int)valR);
        com.pixels.push_back(pixel);
    }

    write_file(com, part_file);
}

void flip(Header p1, const char *part_file) {
    Header flipped = p1;
    flipped.pixels.clear();
    float valB;
    float valG;
    float valR;
    int max = p1.width * p1.height;
    for (int i = 0; i < max; i++) {
        valB = (int)p1.pixels[max - 1 - i].blue;
        valG = (int)p1.pixels[max - 1 - i].green;
        valR = (int)p1.pixels[max - 1 - i].red;
        Pixel pixel((int)valB, (int)valG, (int)valR);
        flipped.pixels.push_back(pixel);
    }

    write_file(flipped, part_file);
}

void check(Header file1, Header file2) {
    for (int i = 0; i < file1.width * file1.height; i++) {
        if ((int)file1.pixels[i].blue == (int)file2.pixels[i].blue && (int)file1.pixels[i].green == (int)file2.pixels[i].green && (int)file1.pixels[i].red == (int)file2.pixels[i].red) {
            continue;
        }
        else {
            cout << "False" << endl;
            cout << (int)file1.pixels[i].blue << " != " << (int)file2.pixels[i].blue << endl;
            cout << (int)file1.pixels[i].green << " != " << (int)file2.pixels[i].green << endl;
            cout << (int)file1.pixels[i].red << " != " << (int)file2.pixels[i].red << endl;
        }
    }
    cout << "True" << endl;
}


int main() {
    overlay(read_file("input/layer1.tga"), read_file("input/pattern1.tga"), "output/part30.tga");
    //overlay(read_file("input/pattern2.tga"), read_file("input/circles.tga"), "output/part31.tga");
    //overlay(read_file("input/layer1.tga"), read_file("input/pattern1.tga"), "output/part32.tga");

    check(read_file("output/part5.tga"), read_file("output/part30.tga"));

    /*
    Two images
    combine (from task 9)

    One image
    screen multiply subtract overlay

    One number
    addred addgreen addblue scalered scalegreen scaleblue

    No arguments
    onlyred onlygreen onlyblue flip
    */

    //read_file("input/car.tga");
    //write_file(read_file("input/car.tga"), "output/out.tga");

    // top layer is p1
    // bottom layer is p2
    //part#.tga



    //part 1
    multiply(read_file("input/layer1.tga"), read_file("input/pattern1.tga"), "output/part1.tga");

    check(read_file("output/part1.tga"), read_file("examples//EXAMPLE_part1.tga"));

    //part 2
    subtract(read_file("input/car.tga"), read_file("input/layer2.tga"), "output/part2.tga");

    check(read_file("output/part2.tga"), read_file("examples//EXAMPLE_part2.tga"));

    //part 3
    Header p3;
    p3 = multiply_load("input/layer1.tga", "input/pattern2.tga");
    screen(p3, read_file("input/text.tga"), "output/part3.tga");

    check(read_file("output/part3.tga"), read_file("examples//EXAMPLE_part3.tga"));

    //part 4
    Header p4;
    p4 = multiply_load("input/layer2.tga", "input/circles.tga");
    subtract(p4, read_file("input/pattern2.tga"), "output/part4.tga");

    check(read_file("output/part4.tga"), read_file("examples//EXAMPLE_part4.tga"));

    //part 5
    overlay(read_file("input/layer1.tga"), read_file("input/pattern1.tga"), "output/part5.tga");

    check(read_file("output/part5.tga"), read_file("examples//EXAMPLE_part5.tga"));

    //part 6
    addgreen(read_file("input/car.tga"), 200, "output/part6.tga");

    check(read_file("output/part6.tga"), read_file("examples//EXAMPLE_part6.tga"));

    //part 7
    scale(read_file("input/car.tga"), 4, 0, "output/part7.tga");

    check(read_file("output/part7.tga"), read_file("examples//EXAMPLE_part7.tga"));

    //part 8
    separate(read_file("input/car.tga"), "output/part8_r.tga", "output/part8_g.tga", "output/part8_b.tga");

    check(read_file("output/part8_r.tga"), read_file("examples//EXAMPLE_part8_r.tga"));
    check(read_file("output/part8_g.tga"), read_file("examples//EXAMPLE_part8_g.tga"));
    check(read_file("output/part8_b.tga"), read_file("examples//EXAMPLE_part8_b.tga"));


    //part 9
    combine(read_file("input/layer_red.tga"), read_file("input/layer_green.tga"), read_file("input/layer_blue.tga"), "output/part9.tga");

    check(read_file("output/part9.tga"), read_file("examples//EXAMPLE_part9.tga"));

    //part 10
    flip(read_file("input/text2.tga"), "output/part10.tga");

    check(read_file("output/part10.tga"), read_file("examples//EXAMPLE_part10.tga"));


};

