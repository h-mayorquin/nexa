clear all;
clf;

nrProcesses = 6;
plotUnits = 0;
plotActivity = 1;
plotScatterActivity = 0;
plotEventLayer = 0;
plotMDS = 0;
plotVQGrouping = 0;
plotConnections = 0;
plotCombinedTrain = 0;
plotPdistMatrix = 1;
plotCorrMatrix = 0;

nrConnectionsSideA = 16;
nrConnectionsSideB = 17;

% load in network description
indir = 'C:\CurrentProjects\Network\Release\'%Output\recurrClassification\'%Network\Release\';%'C:\Projects\Network\Debug\Simon\NetworkProjects\';%'C:\CurrentProjects\Network\NetworkTests\'%'C:\CurrentProjects\Network\Release\PostLazarus\NetworkTests\';%'C:\CurrentProjects\Network\debug\PostLazarus\NetworkTests\';%'C:\CurrentProjects\Network\Release\';
fileDescr = [indir 'NetworkDetails' num2str(nrProcesses) '.txt'];

fid = fopen(fileDescr)
C = textscan(fid, '%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %*[^\n]',100, 'delimiter',' ');
fclose(fid);

% computer details
nrNodes = str2num(char(C{2}(2)));

% layers details
nrLayers = str2num(char(C{2}(3)));
currentIndex = 0;

%xTrain_20Sub3_1 = load('C:\CurrentProjects\Network\Databases\KrishnaPdatabase\fromMarta\xTrain_20Sub3_1.txt');
% vq background drawing
vqBackground = [];%xTrain_20Sub3_1(1:340);

for i=1:nrLayers
    currentIndex = currentIndex+1;
end

nrMeters = str2num(char(C{2}(4+currentIndex)));
MDSData = [];

% load available data (as generated from meters in network description)
currentFigure = 1;

for i = 1:nrMeters
    type = str2num(char(C{4}(4+i+currentIndex)))
    filename = char(C{2}(4+i+currentIndex));

%     try
%         data = load([indir filename]);
%     catch e
%         disp('Error using load - loading with LoadGrouping')
%         data = LoadGrouping([indir filename]);
%     end

    % plot based on type
    if type == 0 && plotUnits == 1
        disp([indir filename]);
        [time unitData] = LoadWithTimeV2([indir filename]);
%        A = load([indir filename]);
%        unitData = A(2:end,:);
%        time = A(1,:);
        A = [];
        T = [];
        time(end+1) = time(end)+(time(end)-time(end-1));
        for j=1:size(unitData,2)
            A = [A unitData{j}'];
            nrSteps = length(unitData{j});
            timeStep = time(j+1)-time(j);
            if nrSteps == 1
                T = [T time(j)];
            else
                T = [T time(j):timeStep/nrSteps:time(j+1)-timeStep/nrSteps];
            end
        end

        figure(currentFigure);
        plot(T,A);
        currentFigure = currentFigure+1;
        
        if(plotCombinedTrain == 1)
            figure(400);
            hold on;
            plot(T,A);
            hold off;
        end
    
    elseif type == 1 && plotActivity == 1 % activity
        disp([indir filename]);
        %[activityTime activityData] = LoadWithTime([indir filename]);
        %activityData = cell2mat(activityData)';

        A = load([indir filename]);
        activityData = A(:,2:end);
        activityTime = A(:,1);
        figure(currentFigure);
        ActivityPlotLayer(activityTime, activityData, 'none',1)
        title(filename)
        currentFigure = currentFigure+1;
        
        if plotScatterActivity == 1
            
            X = [];
            Y = [];
            for i=1:size(activityData,2)
                ts=find(activityData(:,i) == 1);
                X = [X activityTime(ts')'];
                Y = [Y repmat(i,1,length(ts))];
            end
            
            figure(currentFigure);
            scatter(X,Y,3,'filled');
            xlabel('Time');
            ylabel('Unit nr');

            %%scatter
            currentFigure = currentFigure + 1;
        end
        
        if plotPdistMatrix == 1
            
            figure(currentFigure);
            imagesc(squareform(pdist(activityData)));
            colorbar;
            currentFigure = currentFigure + 1;
        end
        
        if plotCorrMatrix == 1
            
            figure(currentFigure);
            imagesc(corr(activityData'));
            colorbar;
            currentFigure = currentFigure + 1;
        end
        
        %figure(currentFigure)
        %ReprCorrMatr(activityData);
        %currentFigure = currentFigure+1;
    elseif type == 2 && plotEventLayer == 1 % eventlayer
        if(strcmp(filename(1:3),'mds') == 1 && plotMDS == 1) % mds scatter plot
            sizeData = 50; % should be stored in description
            [time mdsData sizeXY] = LoadWithTime([indir filename]);
            mdsData = cell2mat(mdsData)';
            figure(currentFigure);
            MDSTimePlots(mdsData, sizeData);
            currentFigure = currentFigure+1;
        elseif(strcmp(filename(1:2),'vq') == 1 && plotVQGrouping == 1) % vq grouping
            [vqTime vqData vqSizeXY] = LoadWithTime([indir filename]);
            nrGroups = 4;
%            GroupPlots([indir filename]);
            figure(currentFigure);
            GroupTimePlot2d(vqData,nrGroups,vqBackground);
            currentFigure = currentFigure+1;
        end
    elseif type == 3 && plotConnections == 1% connection(s)
        
        if(exist([indir filename], 'file'))
%             [timeData connectionData rawData] = LoadWithTime([indir filename]);
%             connectionData = cell2mat(connectionData)';
%             size2d = sqrt(size(connectionData,2));
%             figure(currentFigure);
%             nrPlots = size(connectionData,1);
%             for m=1:nrPlots
%                 subplot(1,nrPlots,m);
%                 if(size2d>10)
%                     imagesc(reshape(connectionData(m,:),size2d,size2d)');
%                 else
%                     imagesc(connectionData(m,:));
%                 end
%                 
%                 colorbar;
%             end

            A = load([indir filename]);
            figure(currentFigure);

            nrPlots = size(connectionData,1);
            imagesc(connectionData);
            colorbar;

            clf; hold on;
            
            for m=1:nrConnectionsSideA
                for n=1:nrConnectionsSideB
                    plot(A(n:nrConnectionsSideB:end,m));
                end
            end


            %for m=1:nrPlots
            %    subplot(1,nrPlots,m);
                %if(size2d>10)
                %    imagesc(reshape(connectionData(m,:),size2d,size2d)');
                %else
                %    imagesc(connectionData(m,:));
                %end
                                
              %  colorbar;
            %end
            
            currentFigure = currentFigure+1;
        end
       % WeightsTimePlots
    end
end