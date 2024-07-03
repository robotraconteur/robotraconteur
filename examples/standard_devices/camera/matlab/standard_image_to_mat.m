function im=standard_image_to_mat(rr_image)
    w = rr_image.image_info.width;
    h = rr_image.image_info.height;
    encoding = rr_image.image_info.encoding;

    if encoding == 0x1000
        im_c = reshape(rr_image.data, 3, w, h);
        im = permute(im_c, [3, 2, 1]);
    elseif encoding == 0x1001
        im_c = reshape(rr_image.data, 4, w, h);
        im = permute(im_c, [3, 2, 1]);
        im = im(:, :, 1:3);
    elseif encoding == 0x1002
        im_c = reshape(rr_image.data, 3, w, h);
        im = flip(permute(im_c, [3, 2, 1]), 3);
    elseif encoding == 0x1003
        im_c = reshape(rr_image.data, 4, w, h);
        im = permute(im_c, [3, 2, 1]);
        im = im(:, :, 1:3);
        im = flip(im, 3);
    elseif encoding == 0x2000
        im = reshape(rr_image.data, w, h);
    elseif encoding == 0x2001 || encoding == 0x4000
        data = typecast(rr_image.data, 'uint16');
        im = reshape(data, w, h)';
    else
        error('Unknown encoding');
    end
end
