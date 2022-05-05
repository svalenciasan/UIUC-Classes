#include "Image.h"
#include "StickerSheet.h"

int main() {

  //
  // Reminder:
  //   Before exiting main, save your creation to disk as myImage.png
  //
  Image base_image;
  base_image.readFromFile("expected-saturate.png");
  StickerSheet sticker_sheet(base_image, 3);

  Image sticker;
  sticker.readFromFile("i.png");
  sticker_sheet.addSticker(sticker, 0, 0);
  sticker_sheet.addSticker(sticker, 1000, 200);
  sticker_sheet.addSticker(sticker, 400, 400);

  Image output = sticker_sheet.render();
  output.writeToFile("myImage.png");
  return 0;
}
