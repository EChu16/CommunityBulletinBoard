//
//  CommunityTableViewCell.swift
//  CBB
//
//  Created by Erich Chu on 4/21/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit

class CommunityTableViewCell: UITableViewCell {

    @IBOutlet weak var cellView: UIView!
    @IBOutlet weak var boardNum: UILabel!
    @IBOutlet weak var boardName: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
