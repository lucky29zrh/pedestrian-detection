function [boxes, scores] = vp_nonmax_suppression(win_w, win_h, detections, boxes, scores, treshold)    
    
    window_area = win_w * win_h;
    
    for i=1:detections-1
        for j=i:detections
            
            % compare coordinates
            x_check = abs(boxes(i,1) - boxes(j,1));
            y_check = abs(boxes(i,2) - boxes(j,2));
            
            % check for overlap
            if x_check < win_w && y_check < win_h
                crossover_area = (win_w - x_check) * (win_h - y_check);
                persentage_crossover = crossover_area / window_area;
                
                % if overlap too big, remove weaker detection
                if persentage_crossover > treshold
                    if scores(i) > scores(j) && scores(i) ~= 0 && scores(j) ~= 0
                        scores(j) = 0;
                        boxes(j,:) = 0;
                    elseif scores(i) < scores(j) && scores(i) ~= 0 && scores(j) ~= 0
                        scores(i) = 0;
                        boxes(i,:) = 0;
                    end
                end
            end
        end
    end
    
    % remove zero rows
    scores( ~any(scores, 2), : ) = [];
    boxes( ~any(boxes, 2), : ) = [];
end
